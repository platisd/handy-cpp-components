#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <mqtt/async_client.h>

#include "KeyValueMessage.h"
#include "PublisherSubscriber.h"
#include "Queue.h"

class PahoMqttPublisherSubscriber
    : public PublisherSubscriber
    , public mqtt::callback
{
public:
    /**
     * @brief Construct a new Paho Mqtt Publisher Subscriber object
     *
     * @param asynchronousClient    An mqtt asynchronous client already set up
     * @param incomingMessages      A queue to hangle the incoming messages
     * @param outgoingMessages      A queue to handle the outgoing messages
     * @param topicsToSubscribe     All the topics the MQTT client to subscribe
     */
    PahoMqttPublisherSubscriber(
        mqtt::async_client& asynchronousClient,
        Queue<KeyValueMessage>& incomingMessages,
        Queue<std::pair<long, KeyValueMessage>>& outgoingMessages,
        std::vector<std::string> topicsToSubscribe);

    /* Refer to PublisherSubscriber for documentation */
    std::future<bool> publish(const std::string& topic,
                              const std::string& message) override;
    bool setCallback(
        const std::string& topic,
        std::function<void(const std::string&)> callbackFunction) override;

    /* Refer to mqtt::callback interface for documentation */
    void message_arrived(mqtt::const_message_ptr msg) override;
    void connected(const std::string& cause) override;

    /**
     * @brief Process a single incoming message.
     * Will block until an incoming message is available and if one or more
     * callbacks have been registered for the particular topic, they will be
     * invoked.
     *
     */
    void processIncomingMessage();

    /**
     * @brief Process a single outgoing message.
     * Will block until an outgonig message is available. Once it does it
     * will be sent and will wait until the delivery result becomes available.
     */
    void processOutgoingMessage();

private:
    mqtt::async_client& mAsynchronousClient;
    Queue<KeyValueMessage>& mIncomingMessages;
    Queue<std::pair<long, KeyValueMessage>>& mOutgoingMessages;
    const std::vector<std::string> mTopicsToSubscribe;

    std::mutex mPublishMutex;
    std::mutex mRegisterCallbackMutex;
    std::unordered_map<std::string,
                       std::vector<std::function<void(const std::string&)>>>
        mCallbacks;

    std::unordered_map<long, std::promise<bool>> mUnpublishedMessagePromises;
    long mUnpublishedMessageIndex{0};
};
