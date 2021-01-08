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
    virtual void insert(T t) = 0;

    /**
     * @brief Get a message from the queue and remove it from the queue.
     * If there are no messages, block and wait until there are.
     *
     * @return T    The message
     */
    virtual T pop() = 0;

    /**
     * @brief Get a message from the queue and remove it from the queue.
     * If there are no messages, block and wait until the timeout expires.
     *
     * @param timeout   How long to wait for a message to arrive
     * @return std::optional<T>     The message
     */
    virtual std::optional<T> pop(std::chrono::milliseconds timeout) = 0;
};
