#pragma once
#include <stdint.h>
#include <chrono>

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

enum State {
    WORKING,
    DONE,
    ERROR
};

struct Event {
    DeviceType type;
    uint32_t targetId; // 0 is broadcast
    State state;
    Command command;
    int32_t value;
    
    std::chrono::milliseconds start_time;
    std::chrono::milliseconds end_time;
};

struct EventWrapper {
    int priority;
    Event event;

    Event unwrap() {
        return event;
    };
};

class EventFactory {
public:
    static EventWrapper createEvent(Event e, int priority = 1) {
        return EventWrapper{priority, e};
    };
    static EventWrapper createEvent(DeviceType type, uint32_t targetId, Command command, int32_t value = 0, State state = State::WORKING, int priority = 1) {
        Event e = Event{type, targetId, state, command, value};
        return EventWrapper{priority, e};
    };
};