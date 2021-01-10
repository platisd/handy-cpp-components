#include <memory>

#include <gtest/gtest.h>

#include "MockMqttClient.h"
#include "MockQueue.h"
#include "MqttPublisherSubscriber.h"

using namespace ::testing;

namespace
{
const std::vector<std::string> kTopics{"topic1", "topic2"};

struct MockCallback
{
    MOCK_METHOD(void, run, (const std::string&), ());
};
} // namespace

struct MqttPublisherSubscriberTest : public Test
{
    void SetUp() override
    {
        EXPECT_CALL(mMqttClient, getTopics()).WillOnce(Return(kTopics));
        EXPECT_CALL(mMqttClient, onMessageReceived(_))
            .WillOnce([this](auto callback) { mOnReceiveCallback = callback; });

        mMqttPublisherSubscriber = std::make_unique<MqttPublisherSubscriber>(
            mMqttClient, mIncomingQueue, mOutgoingQueue);
    }

    MockMqttClient mMqttClient;
    MockQueue<KeyValueMessage> mIncomingQueue;
    MockQueue<std::pair<long, KeyValueMessage>> mOutgoingQueue;
    std::unique_ptr<MqttPublisherSubscriber> mMqttPublisherSubscriber;
    std::function<void(const std::string&, const std::string&)>
        mOnReceiveCallback;
};

TEST_F(MqttPublisherSubscriberTest,
       publish_WhenCalledMultipleTimes_WillInsertCorrectElementsToQueue)
{
    const auto firstTopic   = "( ͡° ͜ʖ ͡°)";
    const auto secondTopic  = "ʕ•ᴥ•ʔ";
    const auto dummyMessage = "༼ つ ◕_◕ ༽つ";

    EXPECT_CALL(mOutgoingQueue, insert(_))
        .WillOnce([&](auto element) {
            const auto& [index, message] = element;
            EXPECT_EQ(index, 0); // The first message index
            EXPECT_EQ(message, KeyValueMessage(firstTopic, dummyMessage));
        })
        .WillOnce([&](auto element) {
            const auto& [index, message] = element;
            EXPECT_EQ(index, 1); // The second message index
            EXPECT_EQ(message, KeyValueMessage(secondTopic, dummyMessage));
        });

    mMqttPublisherSubscriber->publish(firstTopic, dummyMessage);
    mMqttPublisherSubscriber->publish(secondTopic, dummyMessage);
}

TEST_F(MqttPublisherSubscriberTest,
       publish_WhenMessageDelivered_WillNotifyWithResultWhenDeliveryIsMade)
{
    const auto dummyTopic   = "( ͡° ͜ʖ ͡°)";
    const auto dummyMessage = "༼ つ ◕_◕ ༽つ";

    auto messageDelivery
        = mMqttPublisherSubscriber->publish(dummyTopic, dummyMessage);

    const auto expectedMessageIndex = 0;
    const auto queueMessage         = std::make_pair(
        expectedMessageIndex, KeyValueMessage(dummyTopic, dummyMessage));
    EXPECT_CALL(mOutgoingQueue, extractElement())
        .WillOnce(Return(queueMessage));
    EXPECT_CALL(mMqttClient, sendAndWaitForDelivery(dummyTopic, dummyMessage))
        .WillOnce(Return(true));

    mMqttPublisherSubscriber->processOutgoingMessage();

    EXPECT_TRUE(messageDelivery.get());
}

TEST_F(MqttPublisherSubscriberTest,
       runOnTopicArrival_WhenTopicDoesNotExist_WillReturnFalse)
{
    const auto dummyTopic    = "( ͡° ͜ʖ ͡°)";
    const auto dummyCallback = [](const std::string&) {};

    const auto result = mMqttPublisherSubscriber->runOnTopicArrival(
        dummyTopic, dummyCallback);
    EXPECT_FALSE(result);
}

TEST_F(MqttPublisherSubscriberTest,
       runOnTopicArrival_WhenTopicExists_WillInvokeCallbackOnTopicArrival)
{
    const auto topic = kTopics.front();
    MockCallback mockCallback;
    const auto topicCallback
        = [&](const std::string& message) { mockCallback.run(message); };

    const auto result
        = mMqttPublisherSubscriber->runOnTopicArrival(topic, topicCallback);
    EXPECT_TRUE(result);

    const auto expectedMessage = "(ಥ﹏ಥ)";
    EXPECT_CALL(mIncomingQueue, extractElement())
        .WillOnce(Return(KeyValueMessage(topic, expectedMessage)));
    EXPECT_CALL(mockCallback, run(expectedMessage));

    mMqttPublisherSubscriber->processIncomingMessage();
}

TEST_F(MqttPublisherSubscriberTest,
       runOnTopicArrival_WhenMultipleCallbacksForSameTopic_WillInvokeAll)
{
    const auto topic = kTopics.front();
    MockCallback firstMockCallback;
    const auto firstTopicCallback
        = [&](const std::string& message) { firstMockCallback.run(message); };
    mMqttPublisherSubscriber->runOnTopicArrival(topic, firstTopicCallback);

    MockCallback secondMockCallback;
    const auto secondTopicCallback
        = [&](const std::string& message) { secondMockCallback.run(message); };
    mMqttPublisherSubscriber->runOnTopicArrival(topic, secondTopicCallback);

    const auto expectedMessage = "(ಥ﹏ಥ)";
    EXPECT_CALL(mIncomingQueue, extractElement())
        .WillOnce(Return(KeyValueMessage(topic, expectedMessage)));

    EXPECT_CALL(firstMockCallback, run(expectedMessage));
    EXPECT_CALL(secondMockCallback, run(expectedMessage));

    mMqttPublisherSubscriber->processIncomingMessage();
}

TEST_F(MqttPublisherSubscriberTest,
       processIncomingMessage_WhenUnknownTopicArrives_WillNotInvokeAnyCallback)
{
    const auto dummyTopic = "( ͡° ͜ʖ ͡°)";
    MockCallback mockCallback;
    const auto topicCallback
        = [&](const std::string& message) { mockCallback.run(message); };

    const auto result = mMqttPublisherSubscriber->runOnTopicArrival(
        dummyTopic, topicCallback);
    EXPECT_FALSE(result); // The user should not expect any callback invocation!

    const auto dummyMessage = "(ಥ﹏ಥ)";
    // This won't happen in practice since we will not be subscribed to unknown
    // topics but even if such a topic arrives, it will be ignored
    EXPECT_CALL(mIncomingQueue, extractElement())
        .WillOnce(Return(KeyValueMessage(dummyTopic, dummyMessage)));
    EXPECT_CALL(mockCallback, run(_)).Times(0);

    mMqttPublisherSubscriber->processIncomingMessage();
}
