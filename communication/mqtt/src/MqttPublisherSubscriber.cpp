#include <algorithm>
#include <cassert>

#include "MqttPublisherSubscriber.h"

MqttPublisherSubscriber::MqttPublisherSubscriber(
    MqttClient& mqttClient,
    Queue<KeyValueMessage>& incomingMessages,
    Queue<std::pair<unsigned long, KeyValueMessage>>& outgoingMessages)
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

std::future<bool> MqttPublisherSubscriber::publish(const std::string& topic,
                                                   const std::string& message)
{
    std::lock_guard<std::mutex> lock(mPublishMutex);
    const auto unpublishedMessageIndex = mUnpublishedMessageIndex++;
    mOutgoingMessages.insert({unpublishedMessageIndex, {topic, message}});

    mUnpublishedMessagePromises[unpublishedMessageIndex] = std::promise<bool>{};

    return mUnpublishedMessagePromises[unpublishedMessageIndex].get_future();
}

bool MqttPublisherSubscriber::runOnTopicArrival(
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

void MqttPublisherSubscriber::processIncomingMessage()
{
    const auto message = mIncomingMessages.extractElement();
    if (mCallbacks.contains(message.key))
    {
        for (const auto& callbackFunction : mCallbacks[message.key])
        {
            callbackFunction(message.value);
        }
    }
}

void MqttPublisherSubscriber::processOutgoingMessage()
{
    const auto& [unpublishedMessageIndex, message]
        = mOutgoingMessages.extractElement();
    const auto result
        = mMqttClient.sendAndWaitForDelivery(message.key, message.value);

    assert(mUnpublishedMessagePromises.contains(unpublishedMessageIndex));
    mUnpublishedMessagePromises[unpublishedMessageIndex].set_value(result);
    mUnpublishedMessagePromises.erase(unpublishedMessageIndex);
}
