set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(mqtt_publisher_subscriber_main mqtt_publisher_subscriber.cpp)
target_link_libraries(mqtt_publisher_subscriber_main
    PRIVATE
    lifo_queue
    mqtt_publisher_subscriber
    key_value_message
    paho_mqtt_client
    Threads::Threads
    )
