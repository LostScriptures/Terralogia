extern "C" {
    #include <freertos/FreeRTOS.h>
    #include <driver/gpio.h>
    #include <esp_log.h>
}

#include <memory>
#include <vector>

#include <Devices.h>
#include <DeviceManager.h>
#include <DataClasses.h>

const char* TAG = "Main";

typedef struct Queues {
    EventQueue in_queue = EventQueue();
    EventQueue out_queue = EventQueue();
} Queues;

void DeviceManagerTask(void* pvParameters) {
    Queues* queues = static_cast<Queues*>(pvParameters);

    NEOPixel pixel(1, 3, GPIO_NUM_13);

    // Create devices
    std::vector<std::unique_ptr<Device>> devices;
    devices.push_back(std::make_unique<NEOPixel>(pixel));

    // Create device manager
    DeviceManager manager(
        std::move(devices),
        queues->out_queue,
        queues->in_queue
    );

    // Handle events indefinitely
    while (true) {
        manager.handleEvent();
    }
}
Queues queue_setup() {
    Queues queues;
    xTaskCreatePinnedToCore(DeviceManagerTask, "DeviceManagerTask", 4096, &queues, 1, NULL, 1);
    return queues;
}
extern "C" void app_main() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Queues queues = queue_setup();
    EventWrapper wrapped_event;
    Event e;

    while (1) {
        queues.out_queue.push(EventFactory::createEvent(
            DeviceType::NEOPIXEL,
            1,
            Command::COLOR,
            COLORS::toInt(COLORS::gamma_scale({55, 205, 252}, 0.3))
        ));
        queues.out_queue.push(EventFactory::createEvent(
            DeviceType::NEOPIXEL,
            1,
            Command::COLOR,
            COLORS::toInt(COLORS::gamma_scale({255, 255, 255}, 0.3), 1)
        ));
        queues.out_queue.push(EventFactory::createEvent(
            DeviceType::NEOPIXEL,
            1,
            Command::COLOR,
            COLORS::toInt(COLORS::gamma_scale({247, 168, 184}, 0.3), 2)
        ));
        vTaskDelay(35 / portTICK_PERIOD_MS);
        
        queues.in_queue.pop(wrapped_event, portMAX_DELAY);
        e = wrapped_event.unwrap();
        
        // ESP_LOGI(TAG, "Received event for device: %d of type: %d with command: %d and status: %d", e.targetId, e.type, e.command, e.state);
    }
}
