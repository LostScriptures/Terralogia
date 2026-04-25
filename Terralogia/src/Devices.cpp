#include <iostream>
#include <thread>
#include <chrono>
#include <format>

#include "Devices.h"
#include "DataClasses.h"

extern "C" {
    #include "led_strip.h"
}

using namespace std;

State Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
    return State::DONE;
}

State Led::update(const Event& task)  {
    switch (task.command) {
        case Command::ON:
            gpio_set_level(LED_PIN, 1);
            this->LED_STATE = 1;
            break;

        case Command::OFF:
            gpio_set_level(LED_PIN, 0);
            this->LED_STATE = 0;
            break;

        case Command::TOGGLE:
            gpio_set_level(LED_PIN, !LED_STATE);
            this->LED_STATE = !this->LED_STATE;
            break;

        default:
            return State::ERROR;
    }
    
    return State::DONE;
};

State NEOPixel::update(const Event& task) {
    switch (task.command)
    {
        case Command::ON:
            for (int i = 0; i < length; i++) {
                led_strip_set_pixel(strip, i, 255, 255, 255); // Set to white
            }
            led_strip_refresh(strip);
            return State::DONE;

        case Command::OFF:
            for (int i = 0; i < length; i++) {
                led_strip_set_pixel(strip, i, 0, 0, 0); // Set to black (off)
            }
            led_strip_refresh(strip);
            return State::DONE;

        case Command::COLOR:
            led_strip_set_pixel(strip, task.value >> 24 & 0xFF, task.value >> 16 & 0xFF, task.value >> 8 & 0xFF, task.value & 0xFF); // Extract RGB from value
            led_strip_refresh(strip);
            return State::DONE;
    default:
        return State::ERROR;
    }
};