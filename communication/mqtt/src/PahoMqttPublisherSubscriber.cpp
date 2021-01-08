#include <algorithm>
#include <chrono>
#include <stdexcept>

#include "PahoMqttPublisherSubscriber.h"

namespace
{
constexpr auto kQoS = 1;
} // namespace

using namespace std::chrono_literals;

PahoMqttPublisherSubscriber::PahoMqttPublisherSubscriber(
    mqtt::async_client& asynchronousClient,
    Queue<KeyValueMessage>& incomingMessages,
    std::vector<std::string> topicsToSubscribe)
    : mAsynchronousClient{asynchronousClient}
    , mIncomingMessages{incomingMessages}
    , mTopicsToSubscribe{topicsToSubscribe}
{
    mAsynchronousClient.set_callback(*this);
    mqtt::connect_options opts;
    opts.set_automatic_reconnect(true);
    opts.set_clean_session(false);
    auto connectionToken   = mAsynchronousClient.connect(opts);
    auto connectionSuccess = connectionToken->wait_for(10s);

    if (!connectionSuccess)
    {
        throw std::runtime_error("Paho MQTT Client failed to connect");
    }
}

bool PahoMqttPublisherSubscriber::publish(const std::string& topic,
                                          const std::string& message)
{

    const auto mqttMessage = mqtt::make_message(topic, message);
    mqttMessage->set_qos(kQoS);

    std::lock_guard<std::mutex> lock(mPublishMutex);
    return mAsynchronousClient.publish(mqttMessage) == MQTTASYNC_SUCCESS;
}

bool PahoMqttPublisherSubscriber::setCallback(
    const std::string& topic,
    std::function<void(const std::string&)> callbackFunction)
{
    const auto topicExists
        = std::find(mTopicsToSubscribe.begin(), mTopicsToSubscribe.end(), topic)
          != mTopicsToSubscribe.end();
    if (!topicExists)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(mRegisterCallbackMutex);
    mCallbacks[topic].emplace_back(callbackFunction);

    return true;
}

void PahoMqttPublisherSubscriber::message_arrived(mqtt::const_message_ptr msg)
{
    mIncomingMessages.insert({msg->get_topic(), msg->get_payload()});
}

void PahoMqttPublisherSubscriber::connected(const std::string& /* cause */)
{
    for (const auto& topic : mTopicsToSubscribe)
    {
        mAsynchronousClient.subscribe(topic, kQoS);
    }
}

void PahoMqttPublisherSubscriber::processIncomingMessage()
{
    const auto message = mIncomingMessages.pop();
    if (mCallbacks.contains(message.key))
    {
        for (const auto& callbackFunction : mCallbacks[message.key])
        {
            callbackFunction(message.value);
        }
    }
}
