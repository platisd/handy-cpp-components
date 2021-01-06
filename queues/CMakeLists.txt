add_library(queue_interface INTERFACE)
target_include_directories(queue_interface INTERFACE queue)

add_library(lifo_queue INTERFACE)
target_include_directories(lifo_queue INTERFACE lifo_queue)
target_link_libraries(lifo_queue
    INTERFACE
    queue_interface)
