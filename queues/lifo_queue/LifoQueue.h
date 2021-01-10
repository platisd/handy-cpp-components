#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <vector>

#include "Queue.h"

template<typename T>
class LifoQueue : public Queue<T>
{
public:
    void insert(T t)
    {
        std::lock_guard<std::mutex> lock(mInsertMutex);
        mElements.emplace_back(t);
        mConditionVariable.notify_one();
    }

    T extractElement()
    {
        std::unique_lock<std::mutex> lk(mPopMutex);
        mConditionVariable.wait(lk, [this]() { return !mElements.empty(); });

        return getAndPopLastElement();
    }

    std::optional<T> extractElement(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lk(mPopMutex);
        const auto elementExists = mConditionVariable.wait_for(
            lk, timeout, [this]() { return !mElements.empty(); });

        if (!elementExists)
        {
            return std::nullopt;
        }

        return std::make_optional<T>(getAndPopLastElement());
    }

private:
    std::mutex mInsertMutex;
    std::mutex mPopMutex;
    std::condition_variable mConditionVariable;
    std::vector<T> mElements;

    T getAndPopLastElement()
    {
        std::lock_guard<std::mutex> lock(mInsertMutex);
        const auto lastElement = mElements.back();
        mElements.pop_back();

        return lastElement;
    }
};
