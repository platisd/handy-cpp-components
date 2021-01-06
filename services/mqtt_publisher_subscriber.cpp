#include <iostream>

#include "LifoQueue.h"

int main()
{

    LifoQueue<int> lifoQueue;

    lifoQueue.enqueue(1);
    lifoQueue.enqueue(2);
    lifoQueue.enqueue(3);

    std::cout << lifoQueue.pop() << std::endl;
    std::cout << lifoQueue.pop() << std::endl;
    std::cout << lifoQueue.pop() << std::endl;
    const auto validElement = lifoQueue.pop(std::chrono::seconds(3));
    if (validElement)
    {
        std::cout << validElement.value() << std::endl;
    }
    else
    {
        std::cout << "No more elements" << std::endl;
    }

    return 0;
}
