# AIQ ESP32 Firmware

ESP32 firmware for the Air Quality Monitoring System (AIQ). This code reads multiple air quality sensors, displays data on an I2C LCD, and publishes telemetry to an MQTT broker.

## Overview

The ESP32 firmware performs the following tasks:

- Reads temperature and humidity from the HTU21D sensor
- Reads IAQ, accuracy, and BME680 gas data via the BSEC library
- Reads equivalent CO₂ and TVOC from the SGP30 sensor
- Reads dust concentration from a Sharp GP2Y1010/GP2Y1014 dust sensor
- Displays sensor pages on an I2C LCD 1602
- Uses a push-button to cycle through display pages and wake the screen
- Connects to WiFi and publishes telemetry to MQTT

## Files

- `main.cpp` - Main program loop, sensor scheduling, display management, button handling, MQTT publishing
- `MqttHandler.cpp` / `MqttHandler.h` - WiFi and MQTT setup, broker discovery, publish function, reconnect logic
- `HTU21D.cpp` / `HTU21D.h` - HTU21D temperature/humidity sensor support
- `BME680.cpp` / `BME680.h` - BME680 sensor support via BSEC
- `SGP30.cpp` / `SGP30.h` - SGP30 air quality sensor support
- `SharpDust.cpp` / `SharpDust.h` - Sharp dust sensor reading logic
- `LCD.cpp` / `LCD.h` - LCD display control

## Supported Hardware

- ESP32 microcontroller
- HTU21D temperature and humidity sensor (I2C)
- BME680 gas sensor (I2C) with BSEC for IAQ and accuracy
- SGP30 VOC/eCO₂ sensor (I2C)
- Sharp GP2Y10/GP2Y1014 dust sensor (analog)
- 16x2 I2C LCD display
- Button on GPIO 15 for user interaction

## MQTT and Network

The firmware uses `WiFiMulti` to connect to multiple saved WiFi networks.

Configured WiFi networks in `MqttHandler.cpp`:

- `Caministii` / `xxxxxxxxx`
- `Xiaomi 14T` / `xxxxxxxxx`

MQTT configuration:

- Broker hostname: `raspberrypi`
- Broker port: `1883`
- Topic: `aqm/living/telemetry`

The code uses mDNS to resolve `raspberrypi` to an IP address and then publishes JSON telemetry to MQTT.

## Telemetry Payload

Published JSON format:

```json
{
  "temp": 24.5,
  "hum": 45.2,
  "iaq": 123.0,
  "acc": 3.0,
  "co2": 400.0,
  "tvoc": 15.0,
  "dust": 0.05
}
```

## Features

- Non-blocking timing for sensor reads and MQTT publish
- LCD display with multiple pages:
  - Page 1: Temperature / Humidity
  - Page 2: IAQ / Accuracy
  - Page 3: CO₂ / TVOC
  - Page 4: Dust reading
- Auto power-off display after 15 seconds of inactivity
- Button-triggered page switching and connection retry
- MQTT health checks and automatic reconnect logic

## Requirements

Install the following Arduino/ESP32 libraries:

- `WiFi` / `WiFiMulti`
- `PubSubClient`
- `ESPmDNS`
- `LiquidCrystal_I2C`
- `Wire`
- BSEC library for BME680

## Deployment

1. Open the project in the Arduino IDE or PlatformIO.
2. Select the ESP32 board and correct serial port.
3. Ensure the required libraries are installed.
4. Flash the firmware to the ESP32.
5. Power on the sensors and LCD.
6. Verify MQTT broker connectivity and view published telemetry.

## Notes

- The network and broker credentials are hard-coded in `MqttHandler.cpp`.
- The system publishes telemetry every 5 seconds.
- The display uses an interrupt-driven button handler to avoid blocking the main loop.
