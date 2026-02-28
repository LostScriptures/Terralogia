#include <vector>
#include <memory>
#include <queue>

#include <Devices.cpp>

using namespace std;


auto p_cmp = [](const EventWrapper& a,
              const EventWrapper& b) {
    return a.priority > b.priority;
};

typedef priority_queue<EventWrapper, vector<EventWrapper>, decltype(p_cmp)> EventQueue; 

class DeviceManager {
private:
    vector<unique_ptr<Device>> devices;
    
public:
    EventQueue in_queue; /* Rx side for Events */
    EventQueue out_queue; /* Tx side for Events */

    explicit DeviceManager(vector<unique_ptr<Device>>&& devs, EventQueue in_queue, EventQueue out_queue)
        : devices(move(devs)), in_queue(in_queue), out_queue(out_queue) {}

    void handleEvent(const Event& event) {
        for (auto& d : devices) {
            if (d->getType() == event.type && (event.targetId == 0 || d->getId() == event.targetId)) {
                d->update(event);
            }
        }
    }
};