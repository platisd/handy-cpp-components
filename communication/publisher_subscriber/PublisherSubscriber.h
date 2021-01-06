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
     * @brief Subscribe to a topic for the specified callback to be invoked
     *
     * @param topic     The topic to subscribe to
     * @param callback  The callback to invoke when the topic arrives and take
     *                  the topic's message as an argument.
     * @return int      The ID for the callback of the particular topic to use
     *                  for unsubscribing
     */
    virtual int subscribe(const std::string& topic,
                          std::function<void(const std::string&)> callback)
        = 0;

    /**
     * @brief Do not invoke the callback for the specific topic with the
     * particular id
     *
     * @param topic     The topic
     * @param id        The ID of the callback
     * @return true     When there was a callback to unsubscribe
     * @return false    When there was not a callback to unsubscribe
     */
    virtual bool unsubscribe(const std::string& topic, int id) = 0;
};
