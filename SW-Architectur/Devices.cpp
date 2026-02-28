#include <DataClasses.cpp>

class Device {
public:
    virtual DeviceType getType() const = 0;
    virtual uint32_t getId() const = 0;
    virtual void update(const Event& event) = 0;
    virtual ~Device() = default;
};

// Example
class Lights : public Device {
    uint32_t id;

public:
    Lights(uint32_t id) : id(id) {}
    
    DeviceType getType() const override {
        return DeviceType::LIGHT;
    }

    uint32_t getId() const override {
        return id;
    }
    void update(const Event& task) override {
        /* Event Handling */
    };
};
