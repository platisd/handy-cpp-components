// We will only test the FIFO-specific functionality since the rest were
// verified in the LIFO queue test

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FifoQueue.h"

using namespace ::testing;

TEST(FifoQueueTest,
     extractElement_WhenMultipleElements_WillReturnElementsInCorrectOrder)
{
    FifoQueue<int> fifoQueue;
    const auto firstElement  = 32;
    const auto secondElement = 33;
    const auto thirdElement  = 34;

    fifoQueue.insert(firstElement);
    fifoQueue.insert(secondElement);
    fifoQueue.insert(thirdElement);

    {
        InSequence seq;
        EXPECT_EQ(fifoQueue.extractElement(), firstElement);
        EXPECT_EQ(fifoQueue.extractElement(), secondElement);
        EXPECT_EQ(fifoQueue.extractElement(), thirdElement);
    }
}
