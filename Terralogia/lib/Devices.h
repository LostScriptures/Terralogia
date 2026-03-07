#pragma once
#include <cstdint>

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
class Dummy : public Device {
public:
    Dummy(uint32_t id) : Device(id) {}

    DeviceType getType() const override {
        return DeviceType::DUMMY;
    };
    State update(const Event& task) override;
};