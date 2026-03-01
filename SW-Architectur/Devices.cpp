#include <iostream>
#include <thread>
#include <chrono>

#include "Devices.h"
#include "DataClasses.h"

using namespace std;

State Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
    return State::DONE;
}

State Dummy::update(const Event& task)  {
    cout << "ID: " << task.targetId << "\nSTAT: " << task.state << "\nCOM: " << task.command << "\nVAL: " << task.value << endl;
    this_thread::sleep_for(chrono::seconds(5));
    return State::DONE;
};