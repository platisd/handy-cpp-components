#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "KeyValueMessage.h"
#include "MqttClient.h"
#include "PublisherSubscriber.h"
#include "Queue.h"

class MqttPublisherSubscriber : public PublisherSubscriber
{
public:
    /**
     * @brief Construct a new MqttPublisherSubscriber object
     *
     * @param mqttClient        An mqtt asynchronous client already set up
     * @param incomingMessages  A queue to hangle the incoming messages
     * @param outgoingMessages  A queue to handle the outgoing messages
     */
    MqttPublisherSubscriber(
        MqttClient& mqttClient,
        Queue<KeyValueMessage>& incomingMessages,
        Queue<std::pair<long, KeyValueMessage>>& outgoingMessages);

    /* Refer to PublisherSubscriber for documentation */
    std::future<bool> publish(const std::string& topic,
                              const std::string& message) override;
    bool runOnTopicArrival(
        const std::string& topic,
        std::function<void(const std::string&)> callbackFunction) override;

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
    MqttClient& mMqttClient;
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
