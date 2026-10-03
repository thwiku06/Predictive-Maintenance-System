
# Edge-AI Embedded Predictive Maintenance System

An end-to-end TinyML and hardware control system executing real-time machine health classification directly on an ESP32 microcontroller. The system combines multi-layer neural network inference with hard-coded fail-safe hardware overrides to protect industrial motor loads.

---

## Project Overview
This project continuously monitors structural vibration, operational temperature, and AC/DC load current to detect equipment anomalies before catastrophic motor failure occurs. 

Using an onboard multi-layer perceptron (MLP), the system categorises operational states in real time across four distinct classes while maintaining microsecond safety response times via a hardware vibration override circuit.

### **Classification States**
1. **`device_normal`** — Standard operational parameters.
2. **`device_not_running`** — System idle / zero load detected.
3. **`over_heating`** — Elevated operational thermal threshold detected by AI.
4. **`device_error`** — Critical vibration fault / mechanical failure.

---

##  Hardware Component Architecture
* **Microcontroller:** ESP32-C3 / Arduino-compatible SoC
* **Sensors:**
  * **Current Sensor:** ACS712 / Hall-Effect Current Sensor on `GPIO 3`
  * **Vibration Sensor:** SW-420 Mechanical Vibration Sensor on `GPIO 0`
  * **Environment Sensor:** DHT11 Temperature & Humidity Sensor on `GPIO 1`
* **Outputs & Actuators:**
  * **Motor Relay:** High-Current Safety Relay on `GPIO 6`
  * **Alarm:** Active High Buzzer on `GPIO 5`
  * **Display:** 16x2 I2C LCD Screen (`0x27`) on `SDA: GPIO 8`, `SCL: GPIO 9`

---

## Neural Network & Firmware Features
* **TinyML On-Device Inference:** Multi-Layer Perceptron (3 Inputs → 16 ReLU → 8 ReLU → 4 Softmax Output Logits) implemented directly in C++.
* **Auto-Calibration at Boot:** Performs a 50-sample analogue sampling routine on boot to auto-zero current sensor offset drift.
* **Instant Safety Override:** Instantaneous hardware trip logic overrides AI predictions during mechanical shock/shaking to trip the motor relay and sound the alarm immediately.
* **Live Probability Grid:** Visualizes inference confidence levels directly on the 16x2 LCD in real-time.

---

## Repository Structure
├── main.ino              # Primary system loop, ADC sampler & hardware fail-safe logic
├── predictive_model.h    # Quantized weights, bias vectors & standardization factors
└── README.md             # Project documentation
<img width="1600" height="1200" alt="predictivem" src="https://github.com/user-attachments/assets/6b0fecaa-04e1-4d69-809b-4439e42eb214" />


https://github.com/user-attachments/assets/8c15247e-efe3-4c10-8cfd-397775714e60

