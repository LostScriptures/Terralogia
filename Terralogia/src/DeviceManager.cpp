#include <vector>
#include <memory>
#include <queue>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "../lib/DataClasses.h"
#include "../lib/DeviceManager.h"
#include "../lib/TS_PriorityQueue.h"

using namespace std;

DeviceManager::DeviceManager(vector<unique_ptr<Device>>&& devs, EventQueue& in_queue, EventQueue& out_queue)
    : devices(move(devs)), in_queue(&in_queue), out_queue(&out_queue) {};


void DeviceManager::handleEvent() {
    EventWrapper wrapped_event;
    this->in_queue->pop(wrapped_event, portMAX_DELAY);
    Event event = wrapped_event.unwrap();
    
    if (event.type == DeviceType::MANAGER) {
        if (event.command == Command::OFF) {
            this->out_queue->push(
                EventFactory::createEvent(DeviceType::MANAGER, 0, Command::OFF, 0, 0, State::DONE),
                portMAX_DELAY
            );
            vTaskDelete(NULL);
        }
    }

    for (auto& d : devices) {
        if (d->getType() == event.type && (event.targetId == 0 || d->getId() == event.targetId)) {
            event.start_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

            State result = d->update(event);
            event.state = result;
            
            event.end_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
            
            this->out_queue->push(EventFactory::createEvent(event), portMAX_DELAY);
        }
    }
}
