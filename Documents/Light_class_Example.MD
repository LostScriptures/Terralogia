# Light class  

## Light class
### .h
```cpp
#ifndef Light_H
#define Light_H

#include <Arduino.h>
#include "time.h"
#include <Adafruit_NeoPixel.h>

class Light {
  public:
    enum pixel_modei {PIXEL_OFF, PIXEL_dusk, PIXEL_morning};
    Light(int light_output_pin_set, int neopixl_output_pin_set, int number_of_pixels, tm light_start_time_set, tm light_stop_time_set, tm end_of_morning_sun_exposure_set, tm beginning_of_dusk_set);
    int update(tm current_time);
    int secunds_of_day(tm time_struct);
  private:
    pixel_modei pixel_mode = PIXEL_OFF;
    int light_output_pin;
    int neopixl_output_pin;
    int number_of_pixels;
    Adafruit_NeoPixel strip;
    tm light_start_time;
    int t_start_secunds;
    tm light_stop_time;
    int t_stop_secunds;
    tm end_of_morning_sun_exposure;
    int t_end_morning_secunds;
    tm beginning_of_dusk;
    int t_beginning_dusk_secunds;
    int full_Brightness = 255;
    int mostly_Dimmed_Brightness = 75;
    int curent_Brightness = 0;
};
#endif
```
### .cpp
```cpp
#include "Light.h"
#include <Adafruit_NeoPixel.h> 
#include <Arduino.h>
#include <time.h>

int Light::secunds_of_day(tm time_struct){
    return (time_struct.tm_hour * 3600) 
    + (time_struct.tm_min * 60) 
    + time_struct.tm_sec;
} 

Light::Light(int light_output_pin_set, int neopixl_output_pin_set, int number_of_pixels, tm light_start_time_set, tm light_stop_time_set, tm end_of_morning_sun_exposure_set, tm beginning_of_dusk_set)
{
    light_output_pin = light_output_pin_set;
    neopixl_output_pin = neopixl_output_pin_set;
    number_of_pixels = number_of_pixels;
    strip = Adafruit_NeoPixel(number_of_pixels, neopixl_output_pin, NEO_GRB + NEO_KHZ800);
    light_start_time = light_start_time_set;
    light_stop_time = light_stop_time_set;
    end_of_morning_sun_exposure = end_of_morning_sun_exposure_set;
    beginning_of_dusk = beginning_of_dusk_set;
    t_stop_secunds = secunds_of_day(light_stop_time);
    t_end_morning_secunds = secunds_of_day(end_of_morning_sun_exposure);
    t_beginning_dusk_secunds = secunds_of_day(beginning_of_dusk);
    t_start_secunds = secunds_of_day(light_start_time);
}

int Light::update(tm current_time)
{
    int t_now_secunds = secunds_of_day(current_time);
    if(t_now_secunds >= t_stop_secunds || t_now_secunds < t_start_secunds){
        if(pixel_mode == PIXEL_dusk || pixel_mode == PIXEL_morning){
            strip.fill(strip.Color(0, 0, 0));
            strip.show();
            pixel_mode = PIXEL_OFF;
        }
        digitalWrite(light_output_pin, 255); // Full brightness
        curent_Brightness = full_Brightness;
        return curent_Brightness;
    }// Morning dimming
    else if(t_now_secunds >= t_start_secunds && t_now_secunds < t_end_morning_secunds){
        if(pixel_mode == PIXEL_OFF || pixel_mode == PIXEL_dusk){
            strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn off first pixel to avoid ghosting
            strip.setPixelColor(0, strip.Color(255, 5, 15)); // White color on first pixel
            strip.setBrightness(255); // Set brightness to 255%
            strip.show();
            pixel_mode = PIXEL_morning;
        }
        float progress = (float)(t_now_secunds - t_start_secunds) / (float)(t_end_morning_secunds - t_start_secunds);
        float brightness = mostly_Dimmed_Brightness + progress * (full_Brightness - mostly_Dimmed_Brightness);
        digitalWrite(light_output_pin, round(brightness)); // Dimmed brightness
        curent_Brightness = round(brightness);
        return curent_Brightness;
    }// Evening dimming
    else if(t_now_secunds >= t_beginning_dusk_secunds && t_now_secunds < t_stop_secunds){
        if(pixel_mode == PIXEL_OFF || pixel_mode == PIXEL_morning){
            strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn off first pixel to avoid ghosting
            strip.setPixelColor(0, strip.Color(15, 5, 255)); // White color on first pixel
            strip.setBrightness(255); // Set brightness to 255%
            strip.show();
            pixel_mode = PIXEL_dusk;
        }
        float progress = (float)(t_now_secunds - t_beginning_dusk_secunds) / (float)(t_stop_secunds - t_beginning_dusk_secunds);
        float brightness = full_Brightness + progress * (mostly_Dimmed_Brightness - full_Brightness);
        digitalWrite(light_output_pin, round(brightness)); // Dimmed brightness
        curent_Brightness = round(brightness);
        return curent_Brightness;
    }
    return -1;
}
```