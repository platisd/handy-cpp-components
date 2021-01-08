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

    EXPECT_EQ(mLifoQueue.pop(), expectedElement);
}

TEST_F(LifoQueueTest, pop_WhenNoElements_WillBlockUntilElementIsInserted)
{
    auto pop
        = std::async(std::launch::async, [this]() { return mLifoQueue.pop(); });
    auto popStatus = pop.wait_for(10ms);

    EXPECT_EQ(popStatus, std::future_status::timeout);

    mLifoQueue.insert(1);
    // If we exit the test case it means that `pop` has returned
}

TEST_F(LifoQueueTest, pop_WhenNoElementsAndTimeout_WillReturnInvalidElement)
{
    EXPECT_FALSE(mLifoQueue.pop(1ms));
}

TEST_F(LifoQueueTest, pop_WhenElementsInsertedBeforeTimeout_WillReturnElement)
{
    auto pop       = std::async(std::launch::async,
                          [this]() { return mLifoQueue.pop(1h); });
    auto popStatus = pop.wait_for(10ms);

    ASSERT_EQ(popStatus, std::future_status::timeout);

    const auto expectedElement = 442;
    mLifoQueue.insert(expectedElement);
    const auto validElement = pop.get();
    ASSERT_TRUE(validElement);

    EXPECT_EQ(validElement.value(), expectedElement);
}
