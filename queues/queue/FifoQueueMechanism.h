#pragma once

#include <queue>

#include "QueueMechanism.h"

namespace internal
{

template<typename T>
class QueueMechanism<std::queue<T>>
{
public:
    T extract()
    {
        const auto firstElement = mElements.front();
        mElements.pop();

        return firstElement;
    }

    void add(const T& t)
    {
        mElements.emplace(t);
    }

    bool empty() const
    {
        return mElements.empty();
    }

private:
    std::queue<T> mElements{};
};

} // namespace internal
