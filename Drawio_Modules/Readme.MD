# Draw.io Module Library

This library contains reusable Draw.io files for creating electronic schematics. The goal is to provide a **clear, modular, and consistent representation of microcontrollers, sensors, and actuators**. All files are designed to be **imported directly into new schematics** and combined seamlessly.

---

## Folder Structure
```  
Drawio_Modules/  
├── ESP32/  
├── Sensors/  
├── Actuators/  
└── README.md  
```
### **ESP32/**
Contains symbols and templates for ESP32 boards.  
Example:
- `ESP32_DevKitC_V4.drawio` – Complete representation of all relevant GPIOs for the ESP32 DevKitC V4.

### **Sensors/**
Contains modules for various sensors such as temperature, humidity, and 1-Wire sensors.  
Examples:
- `SHT45_Compact.drawio` – Compact representation of the SHT45 sensor.  
- `DS18B20_Compact.drawio` – DS18B20 1-Wire temperature sensor.


### **Actuators/**
Contains modules for actuators and control devices.  
Examples:
- `PWM_Motor_12V.drawio` – PWM motor controller module with separate control (VCC, GND, PWM) and load (12V+ / 12V- OUT) terminals.
- `Relay_Module.drawio` – Typical relay module with control side (VCC, GND, IN) and load side (COM, NO, NC).
- `Heater_12V.drawio` – 12V heater as a resistive load with two terminals (12V+ / 12V-).
- `Fan_12V.drawio` – 12V fan module with two terminals (12V+ / 12V-) representing the load.
- `Light_12V.drawio` – 12V lighting load with two terminals (12V+ / 12V-).
- `Motor1_12V.drawio` – 12V DC motor load with two terminals (12V+ / 12V-).
- `Motor2_12V.drawio` – 12V DC motor load with two terminals (12V+ / 12V-).
- `NeoPixel.drawio` – NeoPixel / WS2812 RGB LED module with three pins (VCC, GND, DIN/Data).

---

## Usage Notes

1. **Importing into Draw.io:**  
   File → Import → Device → Select the desired `.drawio` file.

2. **Copying into new schematics:**  
   Modules can be directly copied into existing schematics and connected.

3. **Naming Convention:**  
   All files are named by **module type and board/sensor name** (`<ModuleType>_<Name>.drawio`) for quick reference and versioning.

4. **Versioning:**  
   Updates or new versions of modules should include a suffix, e.g., `_v2`, to prevent confusion.

---