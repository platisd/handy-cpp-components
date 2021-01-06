add_library(paho_mqtt_publisher_subscriber
    src/PahoMqttPublisherSubscriber.cpp)
target_include_directories(paho_mqtt_publisher_subscriber
    PUBLIC
    include)
target_link_libraries(paho_mqtt_publisher_subscriber
    PUBLIC
    publisher_subscriber_interface)
