#include <iostream>
#include <thread>
#include <chrono>
#include <format>

#include "Devices.h"
#include "DataClasses.h"

extern "C" {
    #include "led_strip.h"
}

using namespace std;

State Lights::update(const Event& task) {
    cout << "Updating Light " << id << " with event type " << task.type << endl;
    return State::DONE;
}

State Led::update(const Event& task)  {
    switch (task.command) {
        case Command::ON:
            gpio_set_level(LED_PIN, 1);
            this->LED_STATE = 1;
            break;

        case Command::OFF:
            gpio_set_level(LED_PIN, 0);
            this->LED_STATE = 0;
            break;

        case Command::TOGGLE:
            gpio_set_level(LED_PIN, !LED_STATE);
            this->LED_STATE = !this->LED_STATE;
            break;

        default:
            return State::ERROR;
    }
    
    return State::DONE;
};

State NEOPixel::update(const Event& task) {
    switch (task.command)
    {
        case Command::ON:
            for (int i = 0; i < length; i++) {
                led_strip_set_pixel(strip, i, 255, 255, 255); // Set to white
            }
            led_strip_refresh(strip);
            return State::DONE;

        case Command::OFF:
            for (int i = 0; i < length; i++) {
                led_strip_set_pixel(strip, i, 0, 0, 0); // Set to black (off)
            }
            led_strip_refresh(strip);
            return State::DONE;

        case Command::COLOR:
            led_strip_set_pixel(strip, task.value >> 24 & 0xFF, task.value >> 16 & 0xFF, task.value >> 8 & 0xFF, task.value & 0xFF); // Extract RGB from value
            led_strip_refresh(strip);
            return State::DONE;
    default:
        return State::ERROR;
    }
};

struct PumpTaskParams {                         
    Pump*       pump;                           
    EventQueue* out_queue;                      
    Event       origin_event;                  
    TickType_t  delay_ticks;                   
};                                              
                                                
static void pump_task(void* pvParameters) {     
    PumpTaskParams* p =                         
        static_cast<PumpTaskParams*>(pvParameters); 
                                                
    vTaskDelay(p->delay_ticks);                 
                                                
    p->pump->finishTask(p->origin_event);       
                                                
    // Zeit abgelaufen → DONE direkt in out_queue (geht an app_main) 
    p->out_queue->push(                         
        EventFactory::createEvent(p->origin_event), 
        portMAX_DELAY                           
    );                                          
                                                
    delete p;                                   
    vTaskDelete(NULL);                          
}                                               
                                                
void Pump::finishTask(const Event& origin_event) { 
    ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, 0);  
    ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);  
    is_running  = false;                        
    task_handle = nullptr;                      
}                                               
                                                
float Pump::ml_per_second_from_PWM(int pwm) {  
    if (pwm <= calib_data[0].pwm)              
        return calib_data[0].ml_per_s;          
    if (pwm >= calib_data[calib_count-1].pwm)  
        return calib_data[calib_count-1].ml_per_s; 
                                                
    for (int i = 0; i < calib_count - 1; i++) { 
        if (pwm >= calib_data[i].pwm &&         
            pwm <= calib_data[i+1].pwm) {       
            float x0 = calib_data[i].pwm;       
            float y0 = calib_data[i].ml_per_s;  
            float x1 = calib_data[i+1].pwm;     
            float y1 = calib_data[i+1].ml_per_s;
            float t  = (pwm - x0) / (x1 - x0); 
            return y0 + t * (y1 - y0);          
        }                                       
    }                                           
    return 0.0f;                                
}                                               
                                                
State Pump::update(const Event& event) {        
    switch (event.command) {                    
                                                
        case Command::SET: {                    
            if (is_running) return State::ERROR;
                                                
            float ml     = static_cast<float>(event.value); 
            float flow   = ml_per_second_from_PWM(pwm_current); 
            if (flow <= 0.0f) return State::ERROR; 
                                                
            float duration_s  = ml / flow;      
            TickType_t ticks  = pdMS_TO_TICKS((uint32_t)(duration_s * 1000.0f)); 
                                                
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_current);   
            ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);              
            is_running = true;                  
                                                
            PumpTaskParams* params = new PumpTaskParams{ 
                this, out_queue, event, ticks   
            };                                  
            xTaskCreate(pump_task, "pump_task", 2048, params, 5, &task_handle); 
            return State::WORKING;              
        }                                       
                                                
        case Command::ON: {                     
            if (is_running) return State::ERROR;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_current);   
            ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);              
            is_running = true;                  
            return State::DONE;                 
        }                                       
                                                
        case Command::OFF: {                    
            if (!is_running) return State::DONE;
            if (task_handle != nullptr) {       
                vTaskDelete(task_handle);       
                task_handle = nullptr;          
            }                                   
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, 0);  
            ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);  
            is_running = false;                 
            return State::DONE;  // Manager bekommt DONE als normaler return
        }                                       
                                                
        case Command::SPEED: {                  
            int new_pwm = event.value;          
            if (new_pwm < min_pwm) new_pwm = 0; 
            if (new_pwm > max_pwm) new_pwm = max_pwm; 
            pwm_current = new_pwm;              
            if (is_running) {                   
                ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, pwm_current); 
                ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);           
            }                                   
            return State::DONE;                 
        }                                       
                                                
        default:                                
            return State::ERROR;                
    }                                           
}
 