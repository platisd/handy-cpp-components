#include <chrono>
#include <future>

#include <gtest/gtest.h>

#include "LifoQueue.h"

using namespace std::chrono_literals;
using namespace ::testing;

struct LifoQueueTest : public Test
{
    LifoQueue<int> mLifoQueue;
};

TEST_F(LifoQueueTest, insert_WhenCalled_WillInsertElement)
{
    const auto expectedElement = 32;
    mLifoQueue.insert(expectedElement);

    EXPECT_EQ(mLifoQueue.extractElement(), expectedElement);
}

TEST_F(LifoQueueTest,
       extractElement_WhenNoElements_WillBlockUntilElementIsInserted)
{
    auto extract       = std::async(std::launch::async,
                              [this]() { return mLifoQueue.extractElement(); });
    auto extractStatus = extract.wait_for(10ms);

    EXPECT_EQ(extractStatus, std::future_status::timeout);

    mLifoQueue.insert(1);
    // If we exit the test case it means that `extract` has returned
}

TEST_F(LifoQueueTest,
       extractElement_WhenNoElementsAndTimeout_WillReturnInvalidElement)
{
    EXPECT_FALSE(mLifoQueue.extractElement(1ms));
}

TEST_F(LifoQueueTest,
       extractElement_WhenElementsInsertedBeforeTimeout_WillReturnElement)
{
    auto extract = std::async(
        std::launch::async, [this]() { return mLifoQueue.extractElement(1h); });
    auto extractStatus = extract.wait_for(10ms);

    ASSERT_EQ(extractStatus, std::future_status::timeout);

    const auto expectedElement = 442;
    mLifoQueue.insert(expectedElement);
    const auto validElement = extract.get();
    ASSERT_TRUE(validElement);

    EXPECT_EQ(validElement.value(), expectedElement);
}
