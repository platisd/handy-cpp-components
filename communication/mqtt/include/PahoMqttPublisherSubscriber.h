#pragma once

#include "PublisherSubscriber.h"

class PahoMqttPublisherSubscriber : public PublisherSubscriber
{
public:
    /* Refer to PublisherSubscriber for documentation */
    bool publish(const std::string& topic, const std::string& message) override;
    int subscribe(const std::string& topic,
                  std::function<void(const std::string&)> callback) override;
    bool unsubscribe(const std::string& topic, int id) override;
};
