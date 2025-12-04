# Pump Class Example
Ein Beispiel wie die Pump Classe Implementiert wird und Benutzt wird.



## Pump class
```cpp
class Pump{
    public:
    Pump(int pump_output_pin_set, int min_pwm_signal_set, float volume_per_second_set, int pwm_channel_set, int pwm_fr_set){
        pump_output_pin = pump_output_pin_set;
        min_pwm_signal = min_pwm_signal_set;
        volume_per_second = volume_per_second_set;
        is_running = false;
        pwm_channel = pwm_channel_set;
        pwm_fr = pwm_fr_set;
        resolution = 8; // 0 -255
        ledcSetup(pwm_channel, pwm_fr, resolution);
        ledcAttachPin(pump_output_pin, pwm_channel);
    }


    void update(){
        if(is_running && (millis() - start_time >= stop_time)){
            ledcWrite(pwm_channel, 0);
            is_running = false;
        }
    }


    bool pump(float ml){
        if (is_running) return false;
        stop_time = (ml / volume_per_second) * 1000.0f;
        start_time = millis();
        is_running = true;
        ledcWrite(pwm_channel, 128);//resulution muss angepasst werden um den entsprechenden ml per second zu bekommen.
        return true;
    }

    bool runns(){
        return is_running;
    }

    private:
    int resolution; 
    int pwm_channel;
    int pwm_fr;
    unsigned long start_time;
    unsigned long stop_time;
    int pump_output_pin;
    int min_pwm_signal;
    float volume_per_second; //in ml per second
    bool is_running;
};
```

## zu nutzen 
```cpp

// ---- Pumpen-Objekt global anlegen ----
Pump p1(1, 10000, 7, 2, 20000);

void setup() {
    // setup bleibt leer, Konstruktor wurde bereits oben ausgeführt
}

void loop() {
    // Pumpe stoppen wenn Zeit vorbei
    if (p1.runns()) {
        p1.update();
    }

    // Beispielbedingung: Pumpe starten
    if (/* pumpe soll angehen */ false) {
        p1.pump(150);   // 150 ml pumpen
    }
}
```
