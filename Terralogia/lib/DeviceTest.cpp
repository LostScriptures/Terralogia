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
    vector<unique_ptr<Device>> devices;
    
    Device *d1 = new Dummy(1);
    Device *d2 = new Dummy(2);
    
    devices.push_back(unique_ptr<Device>(d1));
    devices.push_back(unique_ptr<Device>(d2));
    
    DeviceManager manager(move(devices), out_queue, in_queue);

    // Start device manager thread
    thread DM(dm_thread, &manager);
    
    cout << "Device Manager thread started\n";
    
    // Create and push test event
    EventWrapper e1 = EventFactory::createEvent(DeviceType::DUMMY, 1, Command::ON, 12);
    EventWrapper e2 = EventFactory::createEvent(DeviceType::DUMMY, 2, Command::OFF, 13, 0);
    
    out_queue.push(e1);
    out_queue.push(e2);
    
    cout << "Event pushed to queue\n";
    
    int count = 0;

    // Wait for the event to be processed and print the result
    while (true) {
        
        if (count == 2) {
            cout << "Sending shutdown signal to Device Manager...\n";
            EventWrapper shutdown_event = EventFactory::createEvent(DeviceType::MANAGER, 0, Command::OFF, 0, 0);
            out_queue.push(shutdown_event);
        }
        
        Event result = in_queue.wait_and_pop().unwrap();
        
        if (result.type == DeviceType::MANAGER && result.command == Command::OFF && result.state == State::DONE) {
            cout << "Received shutdown signal from Device Manager. Exiting...\n";
            break;
        }
        
        chrono::system_clock::time_point start_tp = chrono::system_clock::time_point(chrono::milliseconds(result.start_time));
        chrono::system_clock::time_point end_tp = chrono::system_clock::time_point(chrono::milliseconds(result.end_time));

        cout << "Event processed from " << result.type << " with id: " << result.targetId << " state: " << result.state << endl;
        cout << "Event start time: " << format("{:.22s}", format("{:%F %T}", start_tp)) << endl;
        cout << "Event end time: " << format("{:.22s}", format("{:%F %T}", end_tp)) << endl;
        
        count++;
    }

    if (DM.joinable()) {
        DM.join();
    }

    return 0;
}