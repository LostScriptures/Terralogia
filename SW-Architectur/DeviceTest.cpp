#include <iostream>
#include <thread>

#include <Devices.cpp>

// Example
class Dummy : public Device {
    uint32_t id;

public:
    Dummy(uint32_t id) : id(id) {}
    
    DeviceType getType() const override {
        return DeviceType::DUMMY;
    }

    uint32_t getId() const override {
        return id;
    }
    void update(const Event& task) override {
        cout << "ID: " << task.targetId << "\nCOM: " << task.command << "\nVAL: " << task.value;
    };
};

int main() {
    thread t = thread();
    return 0;
}