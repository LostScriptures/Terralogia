#include <iostream>
#include <thread>
#include <chrono>
#include <format>

#include "../lib/Devices.h"
#include "../lib/DataClasses.h"

using namespace std;

State Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
    return State::DONE;
}

State Led::update(const Event& task)  {
    if (task.command == Command::ON) {
        gpio_set_level(LED_PIN, 1);

    } else if (task.command == Command::OFF) {
        gpio_set_level(LED_PIN, 0);
    }
    
    return State::DONE;
};