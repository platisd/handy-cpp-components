#include <algorithm>

#include "PahoMqttPublisherSubscriber.h"

PahoMqttPublisherSubscriber::PahoMqttPublisherSubscriber(
    MqttClient& mqttClient,
    Queue<KeyValueMessage>& incomingMessages,
    Queue<std::pair<long, KeyValueMessage>>& outgoingMessages)
    : mMqttClient{mqttClient}
    , mIncomingMessages{incomingMessages}
    , mOutgoingMessages{outgoingMessages}
    , mTopicsToSubscribe{mqttClient.getTopics()}
{
    mqttClient.onMessageReceived(
        [this](const std::string& topic, const std::string& message) {
            mIncomingMessages.insert({topic, message});
        });
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

bool PahoMqttPublisherSubscriber::runOnTopicArrival(
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
    const auto result
        = mMqttClient.sendAndWaitForDelivery(message.key, message.value);

    mUnpublishedMessagePromises[unpublishedMessageIndex].set_value(result);
    mUnpublishedMessagePromises.erase(unpublishedMessageIndex);
}
