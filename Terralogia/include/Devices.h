#pragma once
#include <cstdint>

extern "C" {
    #include "led_strip.h"
    #include <driver/gpio.h>
    #include <driver/ledc.h>  
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
}

#include "DataClasses.h"


class Device {
protected:
    uint32_t id;

public:
    Device(uint32_t id) : id(id) {}

    virtual DeviceType getType() const = 0;
    uint32_t getId() const { return id; }    // Default implementation
    virtual State update(const Event& event) = 0;

    virtual ~Device() = default;
};

class Lights : public Device {
public:
    Lights(uint32_t id) : Device(id) {}

    DeviceType getType() const override {
        return DeviceType::LIGHT;
    };
    State update(const Event& task) override;
};

// Dummy device for testing
class Led : public Device {
protected:
    gpio_num_t LED_PIN;
    int LED_STATE = 0; // 0 for off, 1 for on

public:
    Led(uint32_t id, gpio_num_t LED_PIN) : Device(id), LED_PIN(LED_PIN) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << LED_PIN),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };

        gpio_config(&io_conf);

        gpio_set_level(LED_PIN, LED_STATE); // Ensure LED is off initially
    }

    DeviceType getType() const override {
        return DeviceType::LED;
    };
    State update(const Event& task) override;
    ~Led() {
        gpio_set_level(LED_PIN, 0); // Ensure LED is off when destroyed
        gpio_reset_pin(LED_PIN); 
    }
};

class NEOPixel : public Device {
protected:
    uint32_t length;
    led_strip_handle_t strip;

public:
    NEOPixel(uint32_t id, uint32_t length, gpio_num_t NEOPIXEL_PIN) : Device(id), length(length) {
        led_strip_config_t strip_config = {
            .strip_gpio_num = NEOPIXEL_PIN,
            .max_leds = length,
            .led_model = LED_MODEL_WS2812,
            .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
            .flags = {
                .invert_out = false
            }
        };

        led_strip_rmt_config_t rmt_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = 10 * 1000 * 1000,
            .mem_block_symbols = 64,
            .flags = {
                .with_dma = false
            }
        };

        led_strip_new_rmt_device(&strip_config, &rmt_config, &strip);
    }

    DeviceType getType() const override {
        return DeviceType::NEOPIXEL;
    };
    State update(const Event& event) override;
};

struct Interpolation_Points {                   
    int pwm;                                    
    float ml_per_s;                             
};                                              

class Pump : public Device {                    
public:                                         
    Pump(uint32_t id,                           
         gpio_num_t pwm_pin,                    
         ledc_channel_t pwm_channel,            
         ledc_timer_t pwm_timer,                
         int pwm_frequency,                     
         int pwm_resolution,                    
         int min_pwm,                           
         int max_pwm,                           
         const Interpolation_Points* calib_data,
         int calib_count,                       
         EventQueue* out_queue)                 
        : Device(id),                           
          pwm_pin(pwm_pin),                     
          pwm_channel(pwm_channel),             
          pwm_timer(pwm_timer),                 
          pwm_frequency(pwm_frequency),         
          pwm_resolution(pwm_resolution),       
          min_pwm(min_pwm),                     
          max_pwm(max_pwm),                     
          calib_data(calib_data),               
          calib_count(calib_count),             
          out_queue(out_queue),                 
          is_running(false),                    
          task_handle(nullptr),                 
          pwm_current(0)                        
    {                                           
        ledc_timer_config_t timer_conf = {      
            .speed_mode      = LEDC_LOW_SPEED_MODE,     
            .duty_resolution = (ledc_timer_bit_t)pwm_resolution, 
            .timer_num       = pwm_timer,       
            .freq_hz         = (uint32_t)pwm_frequency, 
            .clk_cfg         = LEDC_AUTO_CLK    
        };                                      
        ledc_timer_config(&timer_conf);         
                                                
        ledc_channel_config_t channel_conf = {  
            .gpio_num   = pwm_pin,              
            .speed_mode = LEDC_LOW_SPEED_MODE,  
            .channel    = pwm_channel,          
            .intr_type  = LEDC_INTR_DISABLE,    
            .timer_sel  = pwm_timer,            
            .duty       = 0,                    
            .hpoint     = 0                     
        };                                      
        ledc_channel_config(&channel_conf);     
    }                                           
                                                
    DeviceType getType() const override {       
        return DeviceType::PUMP;                
    }                                           
                                                
    State update(const Event& event) override;  
    void finishTask(const Event& origin_event); 
    float ml_per_second_from_PWM(int pwm);      
                                                
    ~Pump() {                                   
        if (task_handle != nullptr)             
            vTaskDelete(task_handle);           
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, 0);    
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel);    
    }                                           
                                                
private:                                        
    gpio_num_t     pwm_pin;                     
    ledc_channel_t pwm_channel;                 
    ledc_timer_t   pwm_timer;                   
    int            pwm_frequency;               
    int            pwm_resolution;              
    int            min_pwm;                     
    int            max_pwm;                     
                                                
    const Interpolation_Points* calib_data;     
    int            calib_count;                 
                                                
    EventQueue*    out_queue;                   
    bool           is_running;                  
    TaskHandle_t   task_handle;                 
    int            pwm_current;                 
};