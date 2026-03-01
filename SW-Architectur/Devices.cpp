#include <iostream>
#include <thread>
#include <chrono>
#include <format>

#include "Devices.h"
#include "DataClasses.h"

using namespace std;

State Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
    return State::DONE;
}

State Dummy::update(const Event& task)  {
    chrono::system_clock::time_point tp = chrono::system_clock::time_point(chrono::milliseconds(task.start_time));
    cout << "-- Dummy Event (" << format("{:.22s}", format("{:%F %T}", tp)) << ") --\n";
    cout << "ID: " << task.targetId << endl;
    cout << "STAT: " << task.state << endl;
    cout << "COM: " << task.command << endl;
    cout << "VAL: " << task.value << endl;
    
    this_thread::sleep_for(chrono::seconds(5));
    return State::DONE;
};