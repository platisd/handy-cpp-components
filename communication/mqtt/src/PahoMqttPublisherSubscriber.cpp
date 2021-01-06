#include "PahoMqttPublisherSubscriber.h"

bool PahoMqttPublisherSubscriber::publish(const std::string& topic,
                                          const std::string& message)
{
    return false;
}

int PahoMqttPublisherSubscriber::subscribe(
    const std::string& topic, std::function<void(const std::string&)> callback)
{
    return 0;
}

bool PahoMqttPublisherSubscriber::unsubscribe(const std::string& topic, int id)
{
    return false;
}
