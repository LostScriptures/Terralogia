# SHT45 Class Example
Ein Beispiel wie die SHT45 Classe Implementiert wird und Benutzt wird.

## SHT45 Class
### .h
```cpp
#ifndef Sht45_H
#define Sht45_H

#include <Arduino.h>
#include "Adafruit_SHT4x.h"
class Sht45 {
  public:
    enum precision_type {SHT4X_LOW_PRECISION, SHT4X_MED_PRECISION, SHT4X_HIGH_PRECISION};
    enum heater_type {SHT4X_NO_HEATER, SHT4X_HIGH_HEATER_1S, SHT4X_HIGH_HEATER_100MS, SHT4X_MED_HEATER_1S, SHT4X_MED_HEATER_100MS,SHT4X_LOW_HEATER_1S, SHT4X_LOW_HEATER_100MS };
    Sht45(int sht45_number_ID_set, precision_type precision_set, heater_type heater_value_set);
    bool ini_sensor(TwoWire *theWire);
    uint32_t get_serial_number();
    float get_temp_value_in_C();
    float get_hum_value();
    bool measure();
    bool measure_precise(precision_type precision);
    bool measure_precise();
    void set_heater(heater_type heater_value_set);
  private:
   Adafruit_SHT4x sht45_sensor;
   int sht45_number_ID;
   int serial_number;
   sensors_event_t temp_value;
   sensors_event_t hum_value;
   precision_type precision;
   heater_type heater_value;
};
#endif
```
### .cpp
```cpp
#include "Sht45.h"
#include "Adafruit_SHT4x.h"

Sht45::Sht45(int sht45_number_ID_set, precision_type precision_set, heater_type heater_value_set) {
    sht45_number_ID = sht45_number_ID_set;
    precision = precision_set;
    heater_value = heater_value_set;
}

bool Sht45::ini_sensor(TwoWire *theWire) {
    if (! sht45_sensor.begin(theWire)) {
        return false;
    }
    serial_number = sht45_sensor.readSerial();
    sht45_sensor.setPrecision(static_cast<sht4x_precision_t>(precision));
    sht45_sensor.setHeater(static_cast<sht4x_heater_t>(heater_value));
    return true;
}

uint32_t Sht45::get_serial_number() {
    return serial_number;
}

float Sht45::get_temp_value_in_C() {
    return temp_value.temperature;
}
float Sht45::get_hum_value() {
    return hum_value.relative_humidity;
}
bool Sht45::measure() {
    if (!sht45_sensor.getEvent(&hum_value, &temp_value)) {
        return false;
    }
    return true;
}
bool Sht45::measure_precise(precision_type precision) {
    sht45_sensor.setPrecision(static_cast<sht4x_precision_t>(precision));
    bool returnvalue = measure();
    sht45_sensor.setPrecision(static_cast<sht4x_precision_t>(this->precision));
    return returnvalue;
}
bool Sht45::measure_precise() {
    sht45_sensor.setPrecision(static_cast<sht4x_precision_t>(SHT4X_HIGH_PRECISION));
    bool returnvalue = measure();
    sht45_sensor.setPrecision(static_cast<sht4x_precision_t>(precision));
    return returnvalue;
}   
void Sht45::set_heater(heater_type heater_value_set) {
    heater_value = heater_value_set;
    sht45_sensor.setHeater(static_cast<sht4x_heater_t>(heater_value));
}
```
## zu nutzen
```cpp
#include <Arduino.h>
#include <Wire.h>
#include "Sht45.h"

//I2C Busse
TwoWire I2C_1 = TwoWire(0);

//SHT45 Sensor instance
Sht45 sensor1(0, Sht45::SHT4X_HIGH_PRECISION, Sht45::SHT4X_NO_HEATER);

void setup() {
    Serial.begin(115200);
    I2C_1.begin(21, 22); // SDA, SCL
    if (!sensor1.ini_sensor(&I2C_1)) {
        Serial.println("Failed to initialize SHT45 sensor!");
        while (1);
    }
    Serial.print("SHT45 Sensor Serial Number: ");
    Serial.println(sensor1.get_serial_number());
}

void loop() {
    static unsigned long lastMillis = 0;
    if (millis() - lastMillis >= 2000) {
        lastMillis = millis();
        if (sensor1.measure()) {
            Serial.print("Temperature: ");
            Serial.print(sensor1.get_temp_value_in_C());
            Serial.print(" °C, Humidity: ");
            Serial.print(sensor1.get_hum_value());
            Serial.println(" %");
        } else {
            Serial.println("Failed to read from SHT45 sensor!");
        }
    }
}
```