extern "C" {
    #include <freertos/FreeRTOS.h>
    #include <driver/gpio.h>
    #include <esp_log.h>
}

#include <memory>
#include <vector>

#include <../lib/Devices.h>
#include <../lib/DeviceManager.h>
#include <../lib/DataClasses.h>

const char* TAG = "Main";

typedef struct Queues {
    EventQueue in_queue = EventQueue();
    EventQueue out_queue = EventQueue();
} Queues;

void DeviceManagerTask(void* pvParameters) {
    Queues* queues = static_cast<Queues*>(pvParameters);

    Led led1(1, GPIO_NUM_15);

    // Create devices
    std::vector<std::unique_ptr<Device>> devices;
    devices.push_back(std::make_unique<Led>(led1));

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
Queues setup() {
    Queues queues;
    xTaskCreatePinnedToCore(DeviceManagerTask, "DeviceManagerTask", 4096, &queues, 1, NULL, 1);
    return queues;
}
extern "C" void app_main() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Queues queues = setup();
    EventWrapper wrapped_event1;
    EventWrapper wrapped_event2;
    Event e1;
    Event e2;

    while (1) {
        queues.out_queue.push(EventFactory::createEvent(DeviceType::LED, 1, Command::ON), 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        queues.in_queue.pop(wrapped_event1, portMAX_DELAY);
        e1 = wrapped_event1.unwrap();

        ESP_LOGI(TAG, "Received event for device: %d of type: %d with command: %d and status: %d", e1.targetId, e1.type, e1.command, e1.state);

        queues.out_queue.push(EventFactory::createEvent(DeviceType::LED, 1, Command::OFF), 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        queues.in_queue.pop(wrapped_event2, portMAX_DELAY);
        e2 = wrapped_event2.unwrap();

        ESP_LOGI(TAG, "Received event for device: %d of type: %d with command: %d and status: %d", e2.targetId, e2.type, e2.command, e2.state);
    }
}