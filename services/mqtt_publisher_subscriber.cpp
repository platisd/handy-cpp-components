#include <iostream>

#include "LifoQueue.h"

int main()
{
    LifoQueue<int> lifoQueue;

    lifoQueue.insert(1);
    lifoQueue.insert(2);
    lifoQueue.insert(3);

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
