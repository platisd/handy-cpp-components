#pragma once

#include <mqtt/async_client.h>

#include "MqttClient.h"

class PahoMqttClient
    : public MqttClient
    , public mqtt::callback
{
public:
    PahoMqttClient(mqtt::async_client& asynchronousClient,
                   const std::vector<std::string>& topicsToSubscribe);

    /* Refer to MqttClient interface for documentation */
    std::vector<std::string> getTopics() const override;
    void onMessageReceived(
        std::function<void(const std::string&, const std::string&)>
            callWhenMessageReceived) override;
    bool sendAndWaitForDelivery(const std::string& topic,
                                const std::string& message) override;

    /* Refer to mqtt::callback interface for documentation */
    void message_arrived(mqtt::const_message_ptr msg) override;
    void connected(const std::string& cause) override;

private:
    mqtt::async_client& mAsynchronousClient;
    const std::vector<std::string> mTopicsToSubscribe;

    std::function<void(const std::string&, const std::string&)>
        mMessageReceivedCallback{nullptr};
};
