#include <algorithm>
#include <chrono>
#include <stdexcept>

#include "PahoMqttPublisherSubscriber.h"

namespace
{
constexpr auto kQoS = 1;

class PahoCallback : public mqtt::iaction_listener
{
public:
    void on_failure(const mqtt::token& /* asyncActionToken */) override
    {
        mDeliveryPromise.set_value(false);
    }

    void on_success(const mqtt::token& /* asyncActionToken */) override
    {
        mDeliveryPromise.set_value(true);
    }

    bool getDeliveryResult()
    {
        return mDeliveryFuture.get();
    }

private:
    std::promise<bool> mDeliveryPromise;
    std::future<bool> mDeliveryFuture{mDeliveryPromise.get_future()};
};
} // namespace

using namespace std::chrono_literals;

PahoMqttPublisherSubscriber::PahoMqttPublisherSubscriber(
    mqtt::async_client& asynchronousClient,
    Queue<KeyValueMessage>& incomingMessages,
    Queue<std::pair<long, KeyValueMessage>>& outgoingMessages,
    std::vector<std::string> topicsToSubscribe)
    : mAsynchronousClient{asynchronousClient}
    , mIncomingMessages{incomingMessages}
    , mOutgoingMessages{outgoingMessages}
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

std::future<bool>
PahoMqttPublisherSubscriber::publish(const std::string& topic,
                                     const std::string& message)
{
    std::lock_guard<std::mutex> lock(mPublishMutex);
    const auto unpublishedMessageIndex = mUnpublishedMessageIndex++;
    mOutgoingMessages.insert({unpublishedMessageIndex, {topic, message}});

    mUnpublishedMessagePromises[unpublishedMessageIndex] = std::promise<bool>{};

    return mUnpublishedMessagePromises[unpublishedMessageIndex].get_future();
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

void PahoMqttPublisherSubscriber::processOutgoingMessage()
{
    const auto& [unpublishedMessageIndex, message] = mOutgoingMessages.pop();

    const auto mqttMessage = mqtt::make_message(message.key, message.value);
    mqttMessage->set_qos(kQoS);

    PahoCallback pahoCallback;
    mAsynchronousClient.publish(mqttMessage, nullptr, pahoCallback);
    const auto result = pahoCallback.getDeliveryResult();

    mUnpublishedMessagePromises[unpublishedMessageIndex].set_value(result);
    mUnpublishedMessagePromises.erase(unpublishedMessageIndex);
}
