find_library(paho-mqtt3a NAMES paho-mqtt3a REQUIRED)
find_library(paho-mqttpp3 NAMES paho-mqttpp3 REQUIRED)

add_library(paho_mqtt_publisher_subscriber
    src/PahoMqttPublisherSubscriber.cpp)
target_include_directories(paho_mqtt_publisher_subscriber
    PUBLIC
    include)
target_link_libraries(paho_mqtt_publisher_subscriber
    PUBLIC
    paho-mqttpp3
    paho-mqtt3a
    queue_interface
    key_value_message
    publisher_subscriber_interface)
