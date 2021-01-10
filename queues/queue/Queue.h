#pragma once

#include <chrono>
#include <optional>

template<typename T>
struct Queue
{
    virtual ~Queue() = default;

    /**
     * @brief Add an element to the queue
     *
     * @param t     The element to add
     */
    virtual void insert(const T& t) = 0;

    /**
     * @brief Get an element from the queue and remove it from the queue.
     * If there are no elements, block and wait until there are.
     *
     * @return T    The element
     */
    virtual T extractElement() = 0;

    /**
     * @brief Get an element from the queue and remove it from the queue.
     * If there are no elements, block and wait until the timeout expires.
     *
     * @param timeout           How long to wait for a element to arrive
     * @return std::optional<T> The element
     */
    virtual std::optional<T> extractElement(std::chrono::milliseconds timeout)
        = 0;
};
