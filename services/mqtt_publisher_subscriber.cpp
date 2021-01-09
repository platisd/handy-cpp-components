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
    LifoQueue<std::pair<long, KeyValueMessage>> outgoingQueue;
    mqtt::async_client mqttAsynchronousClient{"localhost",
                                              "mqtt_publisher_subscriber"};
    PahoMqttPublisherSubscriber mqttPublisherSubscriber{
        mqttAsynchronousClient, incomingQueue, outgoingQueue, kTopics};

    mqttPublisherSubscriber.setCallback(
        "/mqtt-workshop/io", [](const std::string& message) {
            std::cout << "Callback 1: " << message << std::endl;
        });

    mqttPublisherSubscriber.setCallback(
        "/mqtt-workshop/io", [](const std::string& message) {
            std::cout << "Callback 2: " << message << std::endl;
        });

    std::thread mqttReceiver([&mqttPublisherSubscriber]() {
        while (true)
        {
            mqttPublisherSubscriber.processIncomingMessage();
        }
    });

    std::thread mqttSender([&mqttPublisherSubscriber]() {
        while (true)
        {
            mqttPublisherSubscriber.processOutgoingMessage();
        }
    });

    auto publishResult
        = mqttPublisherSubscriber.publish("/blerp", "message from blerp");

    std::cout << "Waiting for publish result" << std::endl;
    const auto published = publishResult.get();
    std::cout << "Publication result: " << (published ? "true" : "false")
              << std::endl;

    mqttSender.join();
    mqttReceiver.join();

    return 0;
}
