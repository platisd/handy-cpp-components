# Handy C++ components

A collection of useful C++ components.

They are utilized for learning purposes (e.g. experimenting with features of the language
as well as different libraries) but also for getting up and running quickly with real projects.

The architectural and design drivers behind them are:
* Reusability
  * Components should expose implementation-agnostic public interfaces
* Testability
  * The business logic for all components should be testable (and unit tested) 
* Simplicity
  * Each component should be simple enough to set up and its code-walkthrough to be
    be feasible during an 1-hour meetup
## Components

### MQTT Publisher Subscriber

A collection of classes that allow you to register callbacks to be invoked upon the reception of
a particular MQTT topic.
The [example](services/mqtt_publisher_subscriber.cpp) illustrates how the component could be used
with two threads that asynchronously send and receive messages, while invoking the relevant callbacks.

## Installing depenencies

All components belong to the same CMake project, which means that to compile them you will
need to fetch all of their dependencies:

* CMake
* C++ compiler able to compile C++20 (e.g. GCC-10 or Clang-10)
* Paho MQTT C and C++ libraries
  * [Instructions for Ubuntu](https://github.com/eclipse/paho.mqtt.cpp#unix-and-linux)
  * [Instructions for Windows](https://github.com/eclipse/paho.mqtt.cpp#windows)
