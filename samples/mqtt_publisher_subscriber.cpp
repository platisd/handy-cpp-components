#include <iostream>

#include "FifoQueue.h"
#include "KeyValueMessage.h"
#include "LifoQueue.h"
#include "MqttPublisherSubscriber.h"
#include "PahoMqttClient.h"

namespace
{
const std::vector<std::string> kTopics{"/grcpp-workshop/io",
                                       "/grcpp-workshop/restart"};
const std::string kMqttTopicToPublish{"/grcpp-workshop/cmd"};
} // namespace

int main()
{
    mqtt::async_client mqttAsynchronousClient{"broker.hivemq.com",
                                              "grcpp_demo_client"};

    PahoMqttClient pahoMqttClient{mqttAsynchronousClient, kTopics};

    LifoQueue<KeyValueMessage> incomingQueue;
    FifoQueue<std::pair<unsigned long, KeyValueMessage>> outgoingQueue;
    MqttPublisherSubscriber mqttPublisherSubscriber{
        pahoMqttClient, incomingQueue, outgoingQueue};

    mqttPublisherSubscriber.runOnTopicArrival(
        kTopics[0], [](const std::string& message) {
            std::cout << "Callback 1: " << message << std::endl;
        });

    mqttPublisherSubscriber.runOnTopicArrival(
        kTopics[0], [](const std::string& message) {
            std::cout << "Callback 2: " << message << std::endl;
        });

    mqttPublisherSubscriber.runOnTopicArrival(
        kTopics[1], [](const std::string& message) {
            std::cout << "Callback 3: " << message << std::endl;
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

    auto publishResult = mqttPublisherSubscriber.publish(
        kMqttTopicToPublish, "message from mqttPublisherSubscriber");

    std::cout << "Waiting for the message to be delivered" << std::endl;
    const auto published = publishResult.get();
    std::cout << "Message " << (published ? "successfully" : "unsuccessfully")
              << " delivered" << std::endl;

    mqttSender.join();
    mqttReceiver.join();

    return 0;
}
