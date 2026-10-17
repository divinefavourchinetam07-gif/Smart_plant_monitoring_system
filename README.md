# 🌿 ESP32 Smart Plant System

An automated plant monitoring and irrigation system built using an ESP32 micro-controller. The system measures ambient temperature, humidity, soil moisture, and light levels, automatically watering the plant when dry soil is detected. To conserve energy, the LCD backlight turns on via motion detection.

---

## 🛠️ Components Used

- **Microcontroller:** ESP32 Board
- **Sensors:**
  - **DHT11:** Temperature and humidity sensor
  - **Soil Moisture Sensor:** Analog sensor (ESP32 ADC Pin)
  - **LDR (Light Dependent Resistor):** Ambient light sensing
  - **PIR Motion Sensor:** Triggers LCD backlight on motion
- **Display:** 20x4 I2C LCD Display
- **Actuator:** 5V Relay Module controlling a water pump

---

## 📌 Pin Mapping

| Component | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **DHT11 Data** | `GPIO 4` | Digital input |
| **Soil Moisture Sensor** | `GPIO 32` | Analog input (0 - 4095) |
| **LDR Sensor** | `GPIO 33` | Analog input |
| **PIR Motion Sensor** | `GPIO 13` | Digital input |
| **Relay Module (Pump)** | `GPIO 25` | Digital output |
| **LCD I2C SDA** | `GPIO 21` | Standard ESP32 SDA pin |
| **LCD I2C SCL** | `GPIO 22` | Standard ESP32 SCL pin |

---

## 🚀 Key Features

1. **Automated Irrigation:** 
   - Activates relay (water pump) when soil moisture falls below the set threshold (`DRY_SOIL_THRESHOLD = 3000`).
   - Runs for a designated pump duration (`3000 ms`) and applies a post-watering cooldown period (`15000 ms`) to let soil absorb water before re-evaluating moisture levels.

2. **Energy-Saving Display:**
   - LCD backlight stays off until motion is detected by the PIR sensor.
   - Automatically turns off backlight after `10 seconds` of inactivity.

3. **Real-time Monitoring:**
   - Displays real-time Temperature (°C), Relative Humidity (%), Soil Moisture (%), and System Status on a 20x4 LCD screen.

---

## ⚙️ Configuration & Thresholds

You can fine-tune system timing and thresholds directly in the code:

```cpp
const int DRY_SOIL_THRESHOLD = 3000; // Trigger pump when raw ADC > 3000
const int PUMP_RUN_TIME = 3000;      // Run pump for 3 seconds per cycle
const int WATERING_COOLDOWN = 15000; // Cooldown period of 15 seconds
const int BACKLIGHT_TIME = 10000;    // Keep display lit for 10 seconds after motion
```

---

## 📚 Required Libraries

Make sure to install the following libraries in Arduino IDE before compiling:

- [Adafruit DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)
- [LiquidCrystal_I2C Library](https://github.com/johnrickman/LiquidCrystal_I2C)
- Adafruit Unified Sensor Library (dependency for DHT)

---

## 📄 License

This project is licensed under the MIT License - feel free to modify and adapt for your own use.
