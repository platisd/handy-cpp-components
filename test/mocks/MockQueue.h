#pragma once

#include <gmock/gmock.h>

#include "Queue.h"

template<typename T>
class MockQueue : public Queue<T>
{
public:
    MOCK_METHOD(void, insert, (T t), (override));
    MOCK_METHOD(T, pop, (), (override));
    MOCK_METHOD(std::optional<T>,
                pop,
                (std::chrono::milliseconds timeout),
                (override));
};
