#pragma once
//#include <queue>
#include <vector>
#include <memory>

#include "DataClasses.h"  // wherever EventWrapper is defined
#include "TS_PriorityQueue.h"
#include "Devices.h"

// Comparator for the priority queue
/*
struct CompareEvent {
    bool operator()(const EventWrapper& a, const EventWrapper& b) const {
        return a.priority > b.priority;
    }
};*/

// Typedef / alias for convenience
using EventQueue = RTOSPriorityQueue<EventWrapper, 3, 10>;

class DeviceManager {
private:
    std::vector<std::unique_ptr<Device>> devices;

public:
    EventQueue* in_queue;
    EventQueue* out_queue;

    explicit DeviceManager(std::vector<std::unique_ptr<Device>>&& devs,
                           EventQueue& in_queue,
                           EventQueue& out_queue);
    void handleEvent();
};