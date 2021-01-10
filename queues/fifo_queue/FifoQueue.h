#pragma once

#include "FifoQueueMechanism.h"
#include "GenericQueue.h"

template<typename T>
class FifoQueue : public internal::GenericQueue<std::queue<T>>
{
};
