# Pump Class Example
Ein Beispiel wie die Pump Classe Implementiert wird und Benutzt wird.



## Pump class
```cpp
struct Interpolation_Points{
    int pwm;
    float ml_per_s;
}

class Pump{
    public:
    Pump(int pump_output_pin_set, int pwm_channel_set, int pwm_fr_set, int resolution_set, const Interpolation_Points* calib_data_set, int calib_count_set,int max_pwm_set,int min_pwm_set){


        pump_output_pin = pump_output_pin_set;
        pwm_channel = pwm_channel_set;
        pwm_fr = pwm_fr_set;
        resolution = resolution_set; //Bsp.: 0 -255

        calib_data = calib_data_set;
        calib_count = calib_count_set;

        ledcSetup(pwm_channel, pwm_fr, resolution);
        ledcAttachPin(pump_output_pin, pwm_channel);

        max_pwm = max_pwm_set;
        min_pwm = min_pwm_set;
        pwm_curent = 0;
        is_running = false;
    }


    bool pump(float ml){
        if (is_running) return false;

        target_ml = ml;
        pumped_ml = 0;
        last_update = millis();


        is_running = true;
        ledcWrite(pwm_channel, pwm_curent);//resulution muss angepasst werden um den entsprechenden ml per second zu bekommen.

        return true;
    }


    void update(){

        if(!is_running)return;

        unsinged long now = millis();
        unsinged long dt = now - last_update;
        if(dt==0)return;

        last_update = now;

        float flow = ml_per_second_from_PWM(pwm_current);

        pumped_ml += flow * (dt/1000.0f);

        if(pumped_ml >= target_ml){
            ledcWrite(pwm_channel, 0);
            is_running = false;
        }
    }

    void set_speed_pwm(int pwm){
        if(pwm < min_pwm) pwm=0;
        if(pwm > max_pwm) pwm = max_pwm;

        update();
        pwm_current = pwm;

        if(is_running){
            ledcWrite(pwm_channel, pwm_current);
        }
    }

    bool runns(){
        return is_running;
    }

    float get_pumped_ml() const {
        return pumped_ml;
    }

    private:

    float ml_per_second_from_PWM(int pwm){
        if(pwm <= calib_data[0].pwm) return calib_data[0].ml_per_s;
        if(pwm >= calib_data[calib_count-1].pwm) return calib_data[calib_count-1].ml_per_s;

        //search intervall
        for(int i =  0; i< calib_count -1; i++){
            if(pwm>= calib_data[i].pwm && pwm<= calib.data[i+1].pwm){
                float x0 = calib_data[i].pwm;
                float y0 = calib_data[i].ml_per_s;
                float x1 = calib_data[i+1].pwm;
                float y1 = calib_data[i+1].ml_per_s;

                float t = (pwm - x0) / (x1-x0);

                return y0 +t * (y1 - y0);
            }
        }
        return 0;
    }

    //Hardware 
    int pump_output_pin;
    int pwm_channel;
    int pwm_fr;
    int resolution;
    int max_pwm;
    int min_pwm;
    //Calibrate data
    const Interpolation_Points* calib_data;
    int calib_count;
    //runtime data
    bool is_running;
    unsigned long last_update;
    float target_ml;
    float pumped_ml;
    int pwm_current;
};
```

## zu nutzen 
```cpp

// Beispiel-Kalibrierwerte (Platzhalter, ersetzt sie später!)
const CalPoint pumpCalib[] = {
    {40,  0.05},
    {80,  0.18},
    {120, 0.32},
    {160, 0.52},
    {200, 0.76},
    {240, 0.95},
    {255, 1.02}
};

Pump pump1(23, 0, 2000, 8, pumpCalib, sizeof(pumpCalib)/sizeof(pumpCalib[0]));

void setup() {

    pump1.set_speed_pwm(180);
    pump1.pump(50);

}

void loop() {
    if (pump1.runns()) {
        pump1.update();
        //zeige geförderte ML
        pump1.get_pumed_ml();
    } 
    
    if(/*pumpe soll angehen*/){
        pump1.pump(150);
    }

    if(/*User ändert geschwindigkeit*/){
        pump1.set_speed_pwm(200);
    }

}
```


## Kalibrier-Sketch (zum Ermitteln der Werte)

Damit misst du ml/s bei verschiedenen PWM-Werten.
Das Ergebnis kopierst du später in die Tabelle.

Du brauchst:
- Messzylinder 
- Waage

Du startest den Sketch → wählst PWM → startest → misst → gibst ml ein.

```cpp
#include <Arduino.h>
int pumpPin = 23;
int channel = 0;
int resolution = 8;
int freq = 20000;


unsigned long startTime;
int testPWM;
bool measuring = false;

int measuredPointCount = 0;
int measuredMaxPoints = 3;
float measuredPoints[3][2] = {
    // {PWM, ml/s}
    {-1,  -1},
    {-1,  -1},
    {-1,  -1}
};
enum State {
    WAIT_FOR_PWM,
    WAIT_FOR_ML
};
State currentState = WAIT_FOR_PWM;

 struct dataswap
 {
        float a;
        float b;
 };
 

void setup() {
    Serial.begin(115200);

    ledcSetup(channel, freq, resolution);
    ledcAttachPin(pumpPin, channel);

    currentState = WAIT_FOR_PWM;
    Serial.println("Kalibrierung gestartet.");
    Serial.println("Bitte gib einen PWM-Wert ein (0–255):");
}

void loop() {

    // ---------------------------
    if (Serial.available()) {

        String input = Serial.readStringUntil('\n');
        input.trim();
        
        bool punktgefunden = false;
        bool isNumber = true;
        for (size_t i = 0; i < input.length(); i++) {
            char c = input.charAt(i);

            if(isDigit(c)) {
                continue;
            }
            if(c == '.' && !punktgefunden){
                punktgefunden = true;
                continue;
            }
            isNumber = false;
            break;
        }

        if(!isNumber|| input.length() == 0) {
            Serial.println("Ungültige Eingabe. Bitte eine Zahl eingeben:");
            return;
        }

        if(currentState == State::WAIT_FOR_PWM && isNumber) {
            testPWM = input.toInt();
            if (testPWM < 0 || testPWM > 255) {
                Serial.println("Ungültig. Bitte 0–255 eingeben:");
                return;
            }
            measuredPoints[measuredPointCount][0] = testPWM;
            Serial.println("Pumpe läuft mit PWM " + String(testPWM) + ". Messung läuft automatisch für 10 Sekunden...");
            ledcWrite(channel, testPWM);
            delay(10000);
            ledcWrite(channel, 0); 
            currentState = State::WAIT_FOR_ML;
            Serial.println("Bitte gib die gemessene Menge in ml ein");
            return;
        }

        if(currentState == State::WAIT_FOR_ML && isNumber) {
            float ml = input.toFloat();
            measuredPoints[measuredPointCount][1] = ml / 10; // ml/s
            Serial.println("Gemessen: " + String(ml) + " ml in 10.0 s → ml/s = " + String(measuredPoints[measuredPointCount][1]));
            measuredPointCount++;
            if(measuredPointCount >= measuredMaxPoints) {
                Serial.println("Kalibrierung abgeschlossen.");
                while(true) {
                    Serial.println("Gemessene Kalibrierungspunkte (PWM, ml/s):");
                    for(int i = 0; i < measuredMaxPoints ; i++) {
                        Serial.println("PWM: " + String(measuredPoints[i][0]) + ", ml/s: " + String(measuredPoints[i][1]));
                    }
                    delay(1000);
                }
            } else {
                currentState = State::WAIT_FOR_PWM;
                Serial.println("Bitte gib nächsten PWM-Wert ein (0–255):");
            }
        }

    }
}
```
Ist wie folgt zu nutzen:

1. **Sketch hochladen und den seriellen Monitor öffnen**
   Baudrate: **115200**

2. **PWM-Wert eingeben**
   Zum Beispiel:

   ```
   150
   ```

   → Die Pumpe startet **sofort** und läuft automatisch **10 Sekunden**.

3. **Während die Pumpe läuft**
   Lass sie einfach pumpen.
   Stelle einen Messbecher darunter.

4. **Nach 10 Sekunden stoppt die Pumpe automatisch**
   Der Monitor sagt nun:

   ```
   Bitte gemessene ml eingeben:
   ```

5. **Gib die Menge ein, die im Messbecher ist**
   Beispiel:

   ```
   27.5
   ```

6. **Der Sketch berechnet automatisch die Durchflussrate in ml/s**
   Du bekommst etwas wie:

   ```
   PWM: 150
   Gemessen: 27.5 ml in 10.0 s
   → ml/s = 2.75
   ```

7. **Danach kannst du sofort den nächsten PWM-Wert testen**
   Gib einfach wieder einen neuen Wert ein:

   ```
   180
   ```

   Und der Vorgang startet erneut.



Du misst mehrere PWM-Werte, z. B.:
Mindestens 15 Werte 

| PWM | ml/s |
| --- | ---- |
| 80  | 1.1  |
| 120 | 1.9  |
| 150 | 2.5  |
| 200 | 3.4  |

Diese Tabelle kannst du dann in deinen Pump-Algorithmus übernehmen.
