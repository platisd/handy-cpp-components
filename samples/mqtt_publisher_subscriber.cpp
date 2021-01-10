#include <iostream>

#include "KeyValueMessage.h"
#include "LifoQueue.h"
#include "MqttPublisherSubscriber.h"
#include "PahoMqttClient.h"

namespace
{
const std::vector<std::string> kTopics{"/mqtt-workshop/io",
                                       "/mqtt-workshop/restart"};
} // namespace

int main()
{
    mqtt::async_client mqttAsynchronousClient{"localhost",
                                              "mqtt_publisher_subscriber"};

    PahoMqttClient pahoMqttClient{mqttAsynchronousClient, kTopics};

    LifoQueue<KeyValueMessage> incomingQueue;
    LifoQueue<std::pair<long, KeyValueMessage>> outgoingQueue;
    MqttPublisherSubscriber mqttPublisherSubscriber{
        pahoMqttClient, incomingQueue, outgoingQueue};

    mqttPublisherSubscriber.runOnTopicArrival(
        "/mqtt-workshop/io", [](const std::string& message) {
            std::cout << "Callback 1: " << message << std::endl;
        });

    mqttPublisherSubscriber.runOnTopicArrival(
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

    std::cout << "Waiting for the message to be delivered" << std::endl;
    const auto published = publishResult.get();
    std::cout << "Message " << (published ? "successfully" : "unsuccessfully")
              << " delivered" << std::endl;

    mqttSender.join();
    mqttReceiver.join();

    return 0;
}
