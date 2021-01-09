#include <chrono>
#include <future>
#include <stdexcept>

#include "PahoMqttClient.h"

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

PahoMqttClient::PahoMqttClient(
    mqtt::async_client& asynchronousClient,
    const std::vector<std::string>& topicsToSubscribe)
    : mAsynchronousClient{asynchronousClient}
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

std::vector<std::string> PahoMqttClient::getTopics() const
{
    return mTopicsToSubscribe;
}

void PahoMqttClient::onMessageReceived(
    std::function<void(const std::string&, const std::string&)>
        callWhenMessageReceived)
{
    mMessageReceivedCallback = callWhenMessageReceived;
}

bool PahoMqttClient::sendAndWaitForDelivery(const std::string& topic,
                                            const std::string& message)
{
    const auto mqttMessage = mqtt::make_message(topic, message);
    mqttMessage->set_qos(kQoS);

    PahoCallback pahoCallback;
    mAsynchronousClient.publish(mqttMessage, nullptr, pahoCallback);

    return pahoCallback.getDeliveryResult();
}

void PahoMqttClient::message_arrived(mqtt::const_message_ptr msg)
{
    if (!mMessageReceivedCallback)
    {
        return;
    }
    mMessageReceivedCallback(msg->get_topic(), msg->get_payload());
}

void PahoMqttClient::connected(const std::string& /* cause */)
{
    for (const auto& topic : mTopicsToSubscribe)
    {
        mAsynchronousClient.subscribe(topic, kQoS);
    }
}
