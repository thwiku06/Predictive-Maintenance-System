#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "predictive_model.h"

// Hardware Pin Assignments
#define DHTPIN 1
#define DHTTYPE DHT11
#define VIB_PIN 0
#define CURRENT_PIN 3
#define BUZZER_PIN 5
#define RELAY_PIN 6

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float zero_current_offset = 1.65f; // Dynamic baseline calibrated at boot

float relu(float x) { return x > 0.0f ? x : 0.0f; }

void softmax(float* input, float* output, int size) {
    float max_val = input[0];
    for (int i = 1; i < size; ++i) {
        if (input[i] > max_val) max_val = input[i];
    }
    float sum = 0.0f;
    for (int i = 0; i < size; ++i) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }
    for (int i = 0; i < size; ++i) output[i] /= sum;
}

// Auto-Calibrated Current Reader
float readCurrentSensor() {
    int raw = analogRead(CURRENT_PIN);
    float voltage = (raw / 4095.0f) * 3.3f;
    float sensitivity_30A = 0.066f;
    
    float current = (voltage - zero_current_offset) / sensitivity_30A;
    current = abs(current);
    
    // Force dataset No_Load baseline if negligible
    if (current < 0.30f) {
        current = 0.041f;
    }
    return current;
}

float readVibrationSensor() {
    int raw = analogRead(VIB_PIN);
    float voltage = (raw / 4095.0f) * 3.3f;
    return (voltage > 0.5f) ? 1.0f : 0.0f;
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Default Relay OPEN/ON

    dht.begin();
    Wire.begin(8, 9);
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Calibrating ADC...");

    // AUTO-CALIBRATION: Sample baseline voltage 50 times at boot (Motor MUST be off)
    float total_voltage = 0;
    for (int i = 0; i < 50; i++) {
        int raw = analogRead(CURRENT_PIN);
        total_voltage += (raw / 4095.0f) * 3.3f;
        delay(10);
    }
    zero_current_offset = total_voltage / 50.0f;

    lcd.clear();
    lcd.print("System Ready!");
    delay(1000);
    lcd.clear();
}

void loop() {
    float raw_temp = dht.readTemperature();
    float raw_vibration = readVibrationSensor();
    float raw_current = readCurrentSensor();

    if (isnan(raw_temp)) raw_temp = 27.97f;

    // Standardize Inputs
    float norm_current = (raw_current - CURRENT_MEAN) / CURRENT_SCALE;
    float norm_vibration = (raw_vibration - VIB_MEAN) / VIB_SCALE;
    float norm_temp = (raw_temp - TEMP_MEAN) / TEMP_SCALE;

    float input_vector[3] = {norm_current, norm_vibration, norm_temp};

    // Forward Pass Layer 1
    float layer1_output[16];
    for (int j = 0; j < 16; ++j) {
        float sum = layer1_bias[j];
        for (int i = 0; i < 3; ++i) sum += input_vector[i] * layer1_weights[i][j];
        layer1_output[j] = relu(sum);
    }

    // Forward Pass Layer 2
    float layer2_output[8];
    for (int j = 0; j < 8; ++j) {
        float sum = layer2_bias[j];
        for (int i = 0; i < 16; ++i) sum += layer1_output[i] * layer2_weights[i][j];
        layer2_output[j] = relu(sum);
    }

    // Forward Pass Layer 3
    float raw_logits[4];
    for (int j = 0; j < 4; ++j) {
        float sum = layer3_bias[j];
        for (int i = 0; i < 8; ++i) sum += layer2_output[i] * layer3_weights[i][j];
        raw_logits[j] = sum;
    }

    float probabilities[4];
    softmax(raw_logits, probabilities, 4);

    const char* class_names[4] = {"device_normal", "device_not_running", "over_heating", "device_error"};

    int predicted_class = 0;
    float max_prob = probabilities[0];
    for (int i = 1; i < 4; ++i) {
        if (probabilities[i] > max_prob) {
            max_prob = probabilities[i];
            predicted_class = i;
        }
    }

    // -------------------------------------------------------------
    // OVERRIDE: VIBRATION TRIPS FAULT STATE
    // -------------------------------------------------------------
    if (raw_vibration > 0.5f) {
        predicted_class = 3;     // Force state to "device_error"
        probabilities[0] = 0.0f; // Force Nrm to 0%
        probabilities[3] = 1.0f; // Force Err to 100%
        
        digitalWrite(BUZZER_PIN, HIGH); // Sound Alarm
        digitalWrite(RELAY_PIN, LOW);   // Trip Relay (Stop Motor)
    } 
    else if (predicted_class == 2 || predicted_class == 3) {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RELAY_PIN, LOW);   // Keep motor stopped on AI fault
    } 
    else {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RELAY_PIN, HIGH);  // Resume Motor
    }

    // Diagnostics (Prints updated state)
    Serial.print("Current: "); Serial.print(raw_current, 3); Serial.print(" A | ");
    Serial.print("Vib: "); Serial.print((int)raw_vibration); Serial.print(" | ");
    Serial.print("Temp: "); Serial.print(raw_temp, 1); Serial.print(" C | ");
    Serial.print("State: "); Serial.println(class_names[predicted_class]);

    // Display probabilities grid on LCD
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Nrm:"); lcd.print((int)(probabilities[0] * 100.0f)); lcd.print("% ");
    lcd.setCursor(8, 0); lcd.print("Off:"); lcd.print((int)(probabilities[1] * 100.0f)); lcd.print("%");
    lcd.setCursor(0, 1); lcd.print("Ovr:"); lcd.print((int)(probabilities[2] * 100.0f)); lcd.print("% ");
    lcd.setCursor(8, 1); lcd.print("Err:"); lcd.print((int)(probabilities[3] * 100.0f)); lcd.print("%");

    delay(100); 
}
