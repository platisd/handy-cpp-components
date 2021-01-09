#pragma once

#include <functional>
#include <string>
#include <vector>

struct MqttClient
{
    virtual ~MqttClient() = default;

    /**
     * @brief Get the topics the client is subscribed to
     *
     * @return std::vector<std::string> The subscribed topics
     */
    virtual std::vector<std::string> getTopics() const = 0;

    /**
     * @brief Set the callback to be invoked when any message is received
     *
     * @param callWhenMessageReceived The callback to invoke
     */
    virtual void onMessageReceived(
        std::function<void(const std::string&, const std::string&)>
            callWhenMessageReceived)
        = 0;

    /**
     * @brief Send a message and wait for its delivery
     *
     * @param topic     The topic of the message
     * @param message   The payload of the message
     * @return true     If the message was delivered successfully
     * @return false    If the message was not delivered successfully
     */
    virtual bool sendAndWaitForDelivery(const std::string& topic,
                                        const std::string& message)
        = 0;
};
