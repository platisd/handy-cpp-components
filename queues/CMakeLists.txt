add_library(generic_queue_interfaces INTERFACE)
target_include_directories(generic_queue_interfaces INTERFACE queue)

add_library(lifo_queue INTERFACE)
target_include_directories(lifo_queue INTERFACE lifo_queue)
target_link_libraries(lifo_queue
    INTERFACE
    generic_queue_interfaces)

add_library(fifo_queue INTERFACE)
target_include_directories(fifo_queue INTERFACE fifo_queue)
target_link_libraries(fifo_queue
    INTERFACE
    generic_queue_interfaces)
