add_executable(mqtt_publisher_subscriber_main mqtt_publisher_subscriber.cpp)
target_link_libraries(mqtt_publisher_subscriber_main
    PRIVATE
    paho-mqttpp3
    paho-mqttc3::MQTTAsync
    )
