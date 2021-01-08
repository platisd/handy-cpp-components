#pragma once

#include <functional>
#include <string>

struct PublisherSubscriber
{
    virtual ~PublisherSubscriber() = default;

    /**
     * @brief Publish the message at the specified topic
     *
     * @param topic     The topic to publish with
     * @param message   The message to publish
     * @return true     When publish succeeded
     * @return false    When publish failed
     */
    virtual bool publish(const std::string& topic, const std::string& message)
        = 0;

    /**
     * @brief Set callback for the specified callback to be invoked when the
     * particular topic arrives
     *
     * @param topic             The topic to subscribe to
     * @param callbackFunction  The callback to invoke when the topic arrives
     *                          and take the topic's message as an argument.
     * @return true             The callback was set correctly
     * @return false            The callback was not set correctly
     */

    virtual bool
    setCallback(const std::string& topic,
                std::function<void(const std::string&)> callbackFunction)
        = 0;
};
