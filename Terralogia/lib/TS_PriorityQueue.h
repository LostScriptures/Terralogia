#pragma once
#include <freertos/queue.h>
#include <freertos/semphr.h>

template<typename T, int PRIORITY_LEVELS, int QUEUE_LENGTH>
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

    bool push(const T& item, int priority, TickType_t timeout = 0)
    {
        if(priority >= PRIORITY_LEVELS)
            priority = PRIORITY_LEVELS - 1;

        if(priority < 0)
            priority = 0;

        xSemaphoreTake(mutex, portMAX_DELAY);
        bool ok = xQueueSend(queues[priority], &item, timeout) == pdTRUE;
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
