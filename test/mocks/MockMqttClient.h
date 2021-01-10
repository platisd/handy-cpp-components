#pragma once

#include <gmock/gmock.h>

#include "MqttClient.h"

class MockMqttClient : public MqttClient
{
public:
    MOCK_METHOD(std::vector<std::string>, getTopics, (), (const, override));
    MOCK_METHOD(void,
                onMessageReceived,
                (std::function<void(const std::string&, const std::string&)>
                     callWhenMessageReceived),
                (override));
    MOCK_METHOD(bool,
                sendAndWaitForDelivery,
                (const std::string& topic, const std::string& message),
                (override));
};
