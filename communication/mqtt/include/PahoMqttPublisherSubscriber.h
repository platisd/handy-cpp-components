#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
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
    PahoMqttPublisherSubscriber(mqtt::async_client& asynchronousClient,
                                Queue<KeyValueMessage>& incomingMessages,
                                std::vector<std::string> topicsToSubscribe);

    /* Refer to PublisherSubscriber for documentation */
    bool publish(const std::string& topic, const std::string& message) override;
    bool setCallback(
        const std::string& topic,
        std::function<void(const std::string&)> callbackFunction) override;

    /* Refer to mqtt::callback interface for documentation */
    void message_arrived(mqtt::const_message_ptr msg) override;
    void connected(const std::string& cause) override;

    void processIncomingMessage();

private:
    mqtt::async_client& mAsynchronousClient;
    Queue<KeyValueMessage>& mIncomingMessages;
    const std::vector<std::string> mTopicsToSubscribe;

    std::mutex mPublishMutex;
    std::mutex mRegisterCallbackMutex;
    std::unordered_map<std::string,
                       std::vector<std::function<void(const std::string&)>>>
        mCallbacks;
};
