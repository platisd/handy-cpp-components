#pragma once

#include "GenericQueue.h"
#include "LifoQueueMechanism.h"

template<typename T>
class LifoQueue : public internal::GenericQueue<std::vector<T>>
{
};
