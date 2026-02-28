#include <iostream>
#include <thread>
#include <queue>
#include <vector>

#include "Devices.h"
#include "DeviceManager.h"  
#include "DataClasses.h"

using namespace std;

// Dummy device for testing
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

void dm_thread(EventQueue in_queue, EventQueue out_queue) {
    Device *d1 = new Dummy(1);
    vector<unique_ptr<Device>> devices;
    devices.push_back(unique_ptr<Device>(d1));

    DeviceManager manager = DeviceManager(move(devices), move(in_queue), move(out_queue));
}

int main() {
    EventQueue out_queue;
    EventQueue in_queue;

    thread DM = thread(dm_thread, out_queue, in_queue);

    return 0;
}