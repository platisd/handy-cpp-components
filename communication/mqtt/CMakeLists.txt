find_library(paho-mqtt3a NAMES paho-mqtt3a REQUIRED)
find_library(paho-mqttpp3 NAMES paho-mqttpp3 REQUIRED)

add_library(mqtt_publisher_subscriber
    src/MqttPublisherSubscriber.cpp)
target_include_directories(mqtt_publisher_subscriber
    PUBLIC
    include)
target_link_libraries(mqtt_publisher_subscriber
    PUBLIC
    queue_interface
    key_value_message
    publisher_subscriber_interface
    mqtt_client_interface)

add_library(paho_mqtt_client src/PahoMqttClient.cpp)
target_include_directories(paho_mqtt_client
    PUBLIC
    include)
target_link_libraries(paho_mqtt_client
    PUBLIC
    paho-mqttpp3
    paho-mqtt3a
    mqtt_client_interface)
