#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <chrono>
#include <format>

#include "Devices.h"
#include "DeviceManager.h"  
#include "DataClasses.h"

using namespace std;

void dm_thread(DeviceManager* manager) {
    while (true) {
        manager->handleEvent();
    }
}

int main() {
    // Create event queues
    EventQueue out_queue(CompareEvent{});
    EventQueue in_queue(CompareEvent{});
    
    // Create test device and device manager
    Device *d1 = new Dummy(1);
    vector<unique_ptr<Device>> devices;
    devices.push_back(unique_ptr<Device>(d1));
    
    DeviceManager manager(move(devices), out_queue, in_queue);

    // Start device manager thread
    thread DM(dm_thread, &manager);
    
    cout << "Device Manager thread started\n";

    // Create and push test event
    EventWrapper e = EventFactory::createEvent(DeviceType::DUMMY, 1, Command::ON, 12);
    out_queue.push(e);

    cout << "Event pushed to queue\n";
    
    Event result = in_queue.wait_and_pop().unwrap();
    chrono::system_clock::time_point start_tp = chrono::system_clock::time_point(chrono::milliseconds(result.start_time));
    chrono::system_clock::time_point end_tp = chrono::system_clock::time_point(chrono::milliseconds(result.end_time));

    cout << "Event processed with state: " << result.state << endl;
    cout << "Event start time: " << format("{:.22s}", format("{:%F %T}", start_tp)) << endl;
    cout << "Event end time: " << format("{:.22s}", format("{:%F %T}", end_tp)) << endl;
    
    return 0;
}