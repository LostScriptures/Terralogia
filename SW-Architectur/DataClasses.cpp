#include <stdint.h>

using namespace std;

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

struct EventWrapper {
    int priority;
    Event event;
};

struct Event {
    DeviceType type;
    uint32_t targetId;   // 0 = broadcast?
    Command command;
    int32_t value;
    
    // Start timestamp
};