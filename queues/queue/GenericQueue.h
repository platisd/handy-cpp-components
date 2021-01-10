#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "Queue.h"
#include "QueueMechanism.h"

namespace internal
{

template<typename Container>
class GenericQueue : public Queue<typename Container::value_type>
{
public:
    using T = typename Container::value_type;

    void insert(const T& t) override
    {
        std::lock_guard<std::mutex> lock(mInsertMutex);
        mQueueMechanism.add(t);
        mConditionVariable.notify_one();
    }

    T extractElement() override
    {
        std::unique_lock<std::mutex> lk(mPopMutex);
        mConditionVariable.wait(lk,
                                [this]() { return !mQueueMechanism.empty(); });

        return extract();
    }

    std::optional<T> extractElement(std::chrono::milliseconds timeout) override
    {
        std::unique_lock<std::mutex> lk(mPopMutex);
        const auto elementExists = mConditionVariable.wait_for(
            lk, timeout, [this]() { return !mQueueMechanism.empty(); });

        if (!elementExists)
        {
            return std::nullopt;
        }

        return std::make_optional<T>(extract());
    }

private:
    std::mutex mInsertMutex;
    std::mutex mPopMutex;
    std::condition_variable mConditionVariable;
    QueueMechanism<Container> mQueueMechanism;

    T extract()
    {
        std::lock_guard<std::mutex> lock(mInsertMutex);

        return mQueueMechanism.extract();
    }
};

} // namespace internal
