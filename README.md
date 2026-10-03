# Edge-AI Embedded Predictive Maintenance System

An ESP32-based predictive maintenance prototype that combines **TinyML, multi-sensor monitoring, and hardware control** to identify abnormal operating conditions in an electric motor.

The system processes sensor data locally on the ESP32 and uses a lightweight neural network to classify the machine's operating state. A separate vibration-based safety mechanism provides an independent hardware response to critical vibration conditions.
-----------------------------------------------------------------------------------------------------------------------------## Project Overview

Predictive maintenance aims to identify abnormal machine conditions before they develop into serious equipment faults.

This prototype demonstrates the concept using three main parameters:

- **Current** — monitors the electrical load
- **Temperature** — monitors thermal conditions
- **Vibration** — detects abnormal mechanical movement

The sensor data is processed by an **MLP-based TinyML model running directly on the ESP32**.

The system classifies the machine into four operating states:

| Classification | Description |
|---|---|
| `device_normal` | Machine operating normally |
| `device_not_running` | Motor is idle or little/no load is detected |
| `over_heating` | Elevated temperature condition |
| `device_error` | Abnormal vibration / potential fault condition |

Hardware Components
Microcontroller
ESP32-C3 / Arduino-compatible ESP32
Sensors
ACS712 / Hall-effect current sensor
SW-420 vibration sensor
DHT11 temperature and humidity sensor
Outputs
Motor relay
Active buzzer
16x2 I2C LCD
Pin Configuration
Component	ESP32 Pin
Current Sensor	GPIO 3
Vibration Sensor	GPIO 0
DHT11	GPIO 1
Motor Relay	GPIO 6
Buzzer	GPIO 5
LCD SDA	GPIO 8
LCD SCL	GPIO 9

LCD I2C Address: 0x27

TinyML Model

The project uses a lightweight Multi-Layer Perceptron (MLP) implemented directly in embedded C++.

Model Architecture
3 Input Features
       ↓
16 Neurons
ReLU Activation
       ↓
8 Neurons
ReLU Activation
       ↓
4 Output Classes
Softmax

The model uses sensor-derived information including:

Current
Temperature
Vibration

The inference is performed locally on the ESP32, allowing the system to make classification decisions without relying on cloud-based AI inference.

Firmware Features
1. Edge-AI Inference

The neural network runs directly on the ESP32.

This demonstrates how machine-learning models can be deployed on resource-constrained embedded hardware rather than sending all sensor data to an external server.

2. Current Sensor Calibration

At startup, the system performs a sampling routine to estimate the current sensor's baseline offset.

This helps compensate for sensor offset before normal operation begins.

3. Vibration Safety Override

The system includes an independent vibration-based safety condition.

If a critical vibration event is detected, the safety logic can activate the motor protection and alarm independently of the AI classification.

This provides an additional protection layer:

AI Classification
       +
Independent Safety Logic
       ↓
Motor Protection
4. LCD Monitoring

The 16x2 I2C LCD provides local feedback about the machine's operating condition and classification.

5. Hardware Control

The ESP32 controls:

Motor relay
Alarm buzzer
LCD status display

based on the detected machine condition and safety logic.

Repository Structure
Edge-AI-Predictive-Maintenance/
│
├── main.ino
├── predictive_model.h
└── README.md
main.ino

Contains the main firmware, including:

Sensor acquisition
Sensor processing
TinyML inference
Classification logic
Safety override
Relay control
Buzzer control
LCD display
predictive_model.h

Contains the neural-network model parameters, including:

Weights
Biases
Standardization parameters
Model constants
Technologies Used
ESP32-C3
Embedded C/C++
TinyML
Multi-Layer Perceptron
Neural Network Inference
Sensor Interfacing
Embedded Control
I2C Communication
Applications

The concepts demonstrated by this project can be applied to:

Motor condition monitoring
Predictive maintenance
Industrial equipment monitoring
Edge-AI systems
Embedded fault detection
Industrial IoT
Machine protection systems
Limitations

This is an experimental prototype and is not intended to replace certified industrial protection or predictive-maintenance systems.

The prototype uses relatively simple sensors and a lightweight classification model. A real industrial implementation would require:

Higher-quality vibration sensors
Calibrated industrial current and temperature sensors
Larger and more representative datasets
Extensive model validation
False-positive and false-negative analysis
Industrial-grade relay and protection hardware
Electrical isolation and safety measures

The SW-420 vibration sensor is used primarily to demonstrate the vibration-detection and safety-override concept.

Future Improvements
Collect a larger real-world motor dataset
Improve vibration sensing using an accelerometer or industrial vibration sensor
Add advanced signal processing
Perform frequency-domain vibration analysis
Improve model accuracy and validation
Add data logging and health-history tracking
Add IoT/cloud monitoring
Add real-time dashboards
Implement adaptive fault thresholds
Develop a custom PCB
Integrate industrial-grade protection hardware
Project Status

Prototype Completed

This project demonstrates the integration of Edge AI, embedded sensing, TinyML inference, and hardware control on an ESP32 platform for predictive-maintenance applications.

Disclaimer

This project is developed for educational and experimental purposes. It is not a certified medical, industrial safety, or machinery-protection system and should not be used as the sole protection mechanism for real industrial equipment.
<img width="1600" height="1200" alt="predictivem" src="https://github.com/user-attachments/assets/25c8e61f-6a2e-49aa-8d5e-980a46d90022" />


https://github.com/user-attachments/assets/75d8ba1a-a4bc-4d2f-bc43-b4382884ebb6



