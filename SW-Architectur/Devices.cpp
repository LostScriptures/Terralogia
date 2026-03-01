#include <iostream>
#include <thread>
#include <chrono>

#include "Devices.h"
#include "DataClasses.h"

using namespace std;

DeviceType Lights::getType() const {
    return DeviceType::LIGHT;
}
uint32_t Lights::getId() const {
    return id;
}

void Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
}

DeviceType Dummy::getType() const {
    return DeviceType::DUMMY;
}

uint32_t Dummy::getId() const {
    return id;
}

void Dummy::update(const Event& task)  {
    cout << "ID: " << task.targetId << "\nCOM: " << task.command << "\nVAL: " << task.value << endl;
    this_thread::sleep_for(chrono::seconds(5));
    exit(0);
};