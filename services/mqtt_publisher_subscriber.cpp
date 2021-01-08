#include <iostream>

#include "KeyValueMessage.h"
#include "LifoQueue.h"
#include "PahoMqttPublisherSubscriber.h"

namespace
{
const std::vector<std::string> kTopics{"/mqtt-workshop/io",
                                       "/mqtt-workshop/restart"};
} // namespace

int main()
{
    LifoQueue<KeyValueMessage> incomingQueue;
    mqtt::async_client mqttAsynchronousClient{"localhost",
                                              "mqtt_publisher_subscriber"};
    PahoMqttPublisherSubscriber mqttPublisherSubscriber{
        mqttAsynchronousClient, incomingQueue, kTopics};

    mqttPublisherSubscriber.setCallback(
        "/mqtt-workshop/io",
        [](const std::string& message) { std::cout << message << std::endl; });

    std::thread mqttReceiver([&mqttPublisherSubscriber]() {
        while (true)
        {
            mqttPublisherSubscriber.processIncomingMessage();
        }
    });

    mqttPublisherSubscriber.publish("/blerp", "message from blerp");

    mqttReceiver.join();

    return 0;
}
