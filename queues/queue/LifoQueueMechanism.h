#pragma once

#include <vector>

#include "QueueMechanism.h"

namespace internal
{

template<typename T>
class QueueMechanism<std::vector<T>>
{
public:
    T extract()
    {
        const auto lastElement = mElements.back();
        mElements.pop_back();

        return lastElement;
    }

    void add(const T& t)
    {
        mElements.emplace_back(t);
    }

    bool empty() const
    {
        return mElements.empty();
    }

private:
    std::vector<T> mElements{};
};

} // namespace internal
