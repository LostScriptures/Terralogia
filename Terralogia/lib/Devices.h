#pragma once
#include <cstdint>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>

#include "DataClasses.h"


class Device {
protected:
    uint32_t id;

public:
    Device(uint32_t id) : id(id) {}

    virtual DeviceType getType() const = 0;
    uint32_t getId() const { return id; }    // Default implementation
    virtual State update(const Event& event) = 0;

    virtual ~Device() = default;
};

class Lights : public Device {
public:
    Lights(uint32_t id) : Device(id) {}

    DeviceType getType() const override {
        return DeviceType::LIGHT;
    };
    State update(const Event& task) override;
};

// Dummy device for testing
class Led : public Device {
protected:
    gpio_num_t LED_PIN;

public:
    Led(uint32_t id, gpio_num_t LED_PIN) : Device(id), LED_PIN(LED_PIN) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << LED_PIN),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };

        gpio_config(&io_conf);

        gpio_set_level(LED_PIN, 1); // Ensure LED is off initially
        vTaskDelay(500 / portTICK_PERIOD_MS); // Short delay to ensure the pin is set before turning it off
        gpio_set_level(LED_PIN, 0); // Ensure LED is off initially
    }

    DeviceType getType() const override {
        return DeviceType::LED;
    };
    State update(const Event& task) override;
    ~Led() {
        gpio_set_level(LED_PIN, 0); // Ensure LED is off when destroyed
        gpio_reset_pin(LED_PIN); 
    }
};
