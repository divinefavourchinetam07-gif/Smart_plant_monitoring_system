#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// --- PIN DEFINITIONS ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define SOIL_PIN 32
#define LDR_PIN 33
#define PIR_PIN 13
#define RELAY_PIN 25

// --- SYSTEM THRESHOLDS (Adjust these to fit your plant) ---
const int DRY_SOIL_THRESHOLD = 3000; // Readings above this trigger the pump (0-4095 scale)
const int PUMP_RUN_TIME = 3000;      // How long the pump runs per cycle (in ms)
const int WATERING_COOLDOWN = 15000; // Time to wait for water to soak in before checking again (in ms)
const int BACKLIGHT_TIME = 10000;    // How long LCD stays lit after motion is detected (in ms)

// --- OBJECT INITIALIZATION ---
DHT dht(DHTPIN, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27, 20, 4); // Make sure 0x27 is your correct I2C address

// --- STATE TIMERS ---
unsigned long lastMotionTime = 0;
unsigned long lastPumpTime = 0;
bool isPumping = false;
bool backlightOn = true;

void setup() {
  Serial.begin(115200);

  // Initialize Input/Output Pins
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure pump starts completely OFF

  // Initialize Sensors & Display
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Smart Plant System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(3000); // Give PIR time to warm up
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. READ ALL SENSORS
  int moistureRaw = analogRead(SOIL_PIN);
  int lightRaw = analogRead(LDR_PIN);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool motionDetected = digitalRead(PIR_PIN);

  // Convert raw moisture to an estimated percentage for the display
  // 4095 is bone dry air, ~1500 is submerged in water
  int moisturePercent = map(moistureRaw, 4095, 1500, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // 2. POWER SAVING LOGIC (PIR & LCD)
  if (motionDetected) {
    lastMotionTime = currentMillis;
    if (!backlightOn) {
      lcd.backlight();
      backlightOn = true;
    }
  }

  // Turn off backlight if time has expired
  if (backlightOn && (currentMillis - lastMotionTime > BACKLIGHT_TIME)) {
    lcd.noBacklight();
    backlightOn = false;
  }

  // 3. SMART WATERING LOGIC
  // If soil is dry, pump is off, and the cooldown period has passed -> Start pumping
  if (!isPumping && (moistureRaw > DRY_SOIL_THRESHOLD)) {
    if (currentMillis - lastPumpTime > WATERING_COOLDOWN) {
      digitalWrite(RELAY_PIN, HIGH);
      isPumping = true;
      lastPumpTime = currentMillis;
    }
  }

  // If pump has run for the designated time -> Stop pumping
  if (isPumping && (currentMillis - lastPumpTime > PUMP_RUN_TIME)) {
    digitalWrite(RELAY_PIN, LOW);
    isPumping = false;
    lastPumpTime = currentMillis; // Reset timer to start the cooldown phase
  }

  // 4. UPDATE LCD SCREEN
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  if (isnan(temp)) lcd.print("--"); else lcd.print(temp, 1);
  lcd.print((char)223); lcd.print("C "); // Prints degree symbol

  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  if (isnan(hum)) lcd.print("--"); else lcd.print(hum, 1);
  lcd.print("% ");

  lcd.setCursor(0, 2);
  lcd.print("Soil: ");
  lcd.print(moisturePercent);
  lcd.print("%   "); // Trailing spaces overwrite leftover old digits

  lcd.setCursor(0, 3);
  if (isPumping) {
    lcd.print("Status: WATERING... ");
  } else {
    lcd.print("Status: STANDBY     "); // Trailing spaces clear the "WATERING" text
  }

  delay(200); // Brief delay for loop stability
}