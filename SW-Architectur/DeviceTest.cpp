#include <iostream>
#include <thread>
#include <queue>
#include <vector>

#include "Devices.h"
#include "DeviceManager.h"  
#include "DataClasses.h"

using namespace std;

void dm_thread(EventQueue& in_queue, EventQueue& out_queue, DeviceManager* manager) {
    while (true) {
        EventWrapper wraped_event = in_queue.wait_and_pop();
        Event event = wraped_event.event;

        manager->handleEvent(event);
    }
}

int main() {
    EventQueue out_queue(CompareEvent{});
    EventQueue in_queue(CompareEvent{});
    
    Device *d1 = new Dummy(1);
    vector<unique_ptr<Device>> devices;
    devices.push_back(unique_ptr<Device>(d1));
    
    DeviceManager manager(move(devices), in_queue, out_queue);

    thread DM(dm_thread, ref(in_queue), ref(out_queue), &manager);
    
    EventWrapper e = EventFactory::createEvent(DeviceType::DUMMY, 1, Command::ON);
    in_queue.push(e);

    printf("Event pushed to queue\n");
    
    DM.join();  // Wait for thread to complete
    
    return 0;
}