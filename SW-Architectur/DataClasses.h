#pragma once
#include <stdint.h>
#

enum DeviceType {
    LIGHT,
    PUMP,
    FAN,
    DUMMY
};

enum Command {
    ON,
    OFF,
    SET,
    GET
};

struct Event {
    DeviceType type;
    uint32_t targetId;   // 0 = broadcast?
    Command command;
    int32_t value;
    
    // Start timestamp
};

struct EventWrapper {
    int priority;
    Event event;
};

class EventFactory {
public:
    static EventWrapper createEvent(DeviceType type, uint32_t targetId, Command command, int32_t value = 0, int priority = 1) {
        Event e = Event{type, targetId, command, value};
        return EventWrapper{priority, e};
    };
};