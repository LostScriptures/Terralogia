#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <concepts>

template<typename T, int PRIORITY_LEVELS, int QUEUE_LENGTH>
requires requires(T t) { t.priority; }
class RTOSPriorityQueue
{
private:

    QueueHandle_t queues[PRIORITY_LEVELS];
    SemaphoreHandle_t mutex;

public:

    RTOSPriorityQueue()
    {
        mutex = xSemaphoreCreateMutex();

        for(int i = 0; i < PRIORITY_LEVELS; i++)
        {
            queues[i] = xQueueCreate(QUEUE_LENGTH, sizeof(T));
        }
    }

    bool push(const T& item, TickType_t timeout = 0)
    {
        int actual_priority = -1;

        if (item.priority >= PRIORITY_LEVELS) {
            actual_priority = PRIORITY_LEVELS - 1;

        } else if (item.priority < 0) {
            actual_priority = 0;

        } else if (actual_priority == -1) {
            xSemaphoreTake(mutex, portMAX_DELAY);
            bool ok = xQueueSend(queues[item.priority], &item, timeout) == pdTRUE;
            xSemaphoreGive(mutex);
    
            return ok;
        }

        xSemaphoreTake(mutex, portMAX_DELAY);
        bool ok = xQueueSend(queues[item.priority], &item, timeout) == pdTRUE;
        xSemaphoreGive(mutex);

        return ok;
    }

    bool pop(T& item, TickType_t timeout = portMAX_DELAY)
    {
        TickType_t start = xTaskGetTickCount();

        while(true)
        {
            for(int p = PRIORITY_LEVELS - 1; p >= 0; p--)
            {
                if(xQueueReceive(queues[p], &item, 0) == pdTRUE)
                    return true;
            }

            if(timeout == 0)
                return false;

            if(timeout != portMAX_DELAY)
            {
                if(xTaskGetTickCount() - start >= timeout)
                    return false;
            }

            vTaskDelay(1);
        }
    }
};
