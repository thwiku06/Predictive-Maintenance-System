#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define VIBRATION_PIN   0   // Digital input from Vibration Sensor
#define DHT_PIN         1   // Digital data input from DHT11
#define MOTOR_IN1       4   // L298N IN1
#define BUZZER_PIN      5   // Active Buzzer (+)
#define MOTOR_IN2       6   // L298N IN2
#define I2C_SDA         8   // ESP32-C3 I2C SDA
#define I2C_SCL         9   // ESP32-C3 I2C SCL

// --- OBJECT DECLARATIONS ---
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timers
unsigned long lastVibrationTime = 0;
unsigned long lastDisplayUpdate = 0;
const unsigned long HOLD_TIME_MS = 1000;      
const unsigned long LCD_REFRESH_MS = 500;     

void setup() {
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); 

 
  lcd.init();
  lcd.backlight();
  lcd.clear();
  dht.begin();

 
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

 
  int vibrationState = digitalRead(VIBRATION_PIN);
  if (vibrationState == HIGH) {
    lastVibrationTime = currentMillis;
  }

  
  bool isShaking = (currentMillis - lastVibrationTime < HOLD_TIME_MS);

  if (isShaking) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

 
  if (currentMillis - lastDisplayUpdate >= LCD_REFRESH_MS) {
    lastDisplayUpdate = currentMillis;

    float tempC = dht.readTemperature();
    float humidity = dht.readHumidity();

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(isnan(tempC) ? 0 : (int)tempC);
    lcd.print("C H:");
    lcd.print(isnan(humidity) ? 0 : (int)humidity);
    lcd.print("%   ");

    lcd.setCursor(0, 1);
    if (isShaking) {
      lcd.print("Vib: SHAKING!");
    } else {
      lcd.print("Vib: OK");
    }
  }
}