#include <iostream>
#include <thread>
#include <queue>
#include <vector>

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
    cout << "Event processed with state: " << result.state << "\n";
    cout << "Event start time: " << result.start_time.count() << " ms\n";
    cout << "Event end time: " << result.end_time.count() << " ms\n";
    
    return 0;
}