/*
  Master Thesis - Air Quality Monitoring System
  Student: Mihai Sebastian-Andrei
  Main Program File: main.cpp

  Setup and loop logic for reading multiple sensors, managing an LCD display, handling user input via a button, and publishing data to an MQTT broker.
  - Display: LCD 1602 (I2C)
  - Sensors: HTU21D (I2C), SGP30 (I2C), BME680 (I2C), Sharp GP2Y10 (Analog)
 */

#include <Arduino.h>
#include <Wire.h>
#include "HTU21D.h" 
#include "bsec.h"
#include "BME680.h"
#include "LCD.h"
#include <LiquidCrystal_I2C.h>
#include "SGP30.h"
#include "SharpDust.h"
#include "MqttHandler.h"

// Flag to indicate display update has been triggered
volatile bool displayTriggered = false;

#define BUTTON_PIN 15

// --- THE BLACKBOARD (Shared Data) ---
// Global variables to hold latest sensor readings
// Sensors updates these variables, the Display logic reads from them.
String latestTempHTU = "T:0.0C";
String latestHumHTU = "H:0.0%";
String latestIAQ = "IAQ:0";
String latestAccuracy = "Acc:0"; 
String latestCO2String = "CO2:0ppm";
String latestTVOCString = "TVOC:0ppb";
String latestDustString = "Dust:0.0"; 

// --- Display Logic Variables ---
volatile bool buttonPressed = false; // Flag set by Interrupt, is set to true when button is pressed
bool isDisplayOn = false;            // Current state of screen
int currentPage = 1;                 // Interface state: 1=Temp/Hum, 2=IAQ, 3=CO2/TVOC, 4=Dust
unsigned long displayStartTime = 0;  // Timestamp when display was turned on or last button press

// Timers for non-blocking sensor reading intervals
unsigned long lastHTURead = 0;       
unsigned long lastSGPTime = 0;
unsigned long lastDustTime = 0;
unsigned long lastMqttPub = 0;

const long mqttInterval = 5000; // Publish to MQTT every 5000ms

// Sensor Data Variables for MQTT Publishing
float valTemp = 0.0;
float valHum = 0.0;
float valIAQ = 0.0;
float valCO2 = 0.0;
float valTVOC = 0.0;
float valDust = 0.0;

/* Interrupt Service Routine for Button Press 
  Sets the buttonPressed flag to true, to be handled in the main loop to avoid doing too much in the ISR.
*/

void IRAM_ATTR onButtonPress() {
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  buttonPressed = true;
}


void setup() {
  Serial.begin(115200);
  Wire.begin(); // I2C intialization
  
  // 1. Initialize HTU21D (Temp/Humidity)
  Serial.print("Intializing HTU21D... ");
  softReset(); 

  // 2. Initialize BME680 (BSEC v1.6 Library)
  Serial.println("Initializing BME680...");
  setupBME680();

  // 3. Initialize SGP30 (TVOC/eCO2)
  Serial.println("Initializing SGP30...");
  setupSGP30();
  
  // 4. Initialize Sharp GP2Y1010AU0F (Dust Sensor)
  Serial.println("SharpDust initialization...");
  setupSharpDust();

  // 5. Initialize LCD
  Serial.println("LCD initialization...");
  setupLCD();
  
  // 6. Initialize MQTT
  Serial.println("MQTT initialization...");
  setupMqtt();

  // Configure button pin with internal pull-up resistor (active low)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach interrupt to falling edge (button press pulls pin to GND)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);

  Serial.println("System Ready. Loop starting.");
}


void loop() {
  unsigned long currentMillis = millis(); // Current time for non-blocking timing

  handleMqtt(); // Handle MQTT client loop

  // # Sensor 1: HTU21D (Sampling Interval: 3000ms) 
  if (currentMillis - lastHTURead > 3000) {
      lastHTURead = currentMillis;
      
      // Acquire raw data and convert to metric units
      uint16_t tRaw = getTemperatureRaw();
      valTemp = calculateTemperature(tRaw);
      latestTempHTU = "T:" + String(calculateTemperature(tRaw))+"C";
      
      uint16_t hRaw = getHumidityRaw();
      valHum = calculateHumidity(hRaw);
      latestHumHTU = "H:" + String(calculateHumidity(hRaw))+"%";
      
      // Debug output
      Serial.print("T: "); Serial.print(latestTempHTU);
      Serial.print(" H: "); Serial.println(latestHumHTU);

      // Update display immediately if currently viewing Page 1
      if (isDisplayOn && currentPage == 1) {
          displayLCDData(latestTempHTU, latestHumHTU,currentPage);
      }
  }
    
  // # Sensor 2: BME680 (BSEC Library State Machine)
  // The BSEC library handles its own scheduling via iaqSensor.run()
  if (readBME680Data() == true) {
      valIAQ = iaqSensor.iaq;
      latestIAQ = "IAQ:" + String(iaqSensor.iaq);
      latestAccuracy = "Acc:" + String(iaqSensor.iaqAccuracy);

      // Update display immediately if currently viewing Page 2
      if (isDisplayOn && currentPage == 2) {
          displayLCDData(latestIAQ, latestAccuracy, currentPage);
      }
  }
  
  // # Sensor 3: SGP30 (Sampling Interval: 3000ms)
  if (currentMillis - lastSGPTime >= 3000) {
        lastSGPTime = currentMillis;
        
        if (readSGP30Data()) {
            Serial.print("CO2: "); Serial.print(latestCO2);
            Serial.print(" ppm | TVOC: "); Serial.println(latestTVOC);Serial.println("ppb");

            valCO2 = latestCO2;
            valTVOC = latestTVOC;
            latestCO2String = "CO2:" + String(latestCO2) + "ppm";
            latestTVOCString = "TVOC:" + String(latestTVOC) + "ppb";
            
            // Update display immediately if currently viewing Page 3
            if (isDisplayOn && currentPage == 3) {
            displayLCDData(latestCO2String, latestTVOCString,currentPage);
            }
        }
  }

  // Sensor 4: SharpDust
   if (currentMillis - lastDustTime >= 1000) {
       lastDustTime = currentMillis;
      
       readSharpDust(); // The timing dance happens here
      
       latestDustString = "Dust:" + String(latestDustDensity) + "mg";
      
       // Update Page 4 LIVE
       if (isDisplayOn && currentPage == 4) {
           displayLCDData(latestDustString, "PM2.5 Sensor", currentPage);
       }
   }


  if (currentMillis - lastMqttPub > mqttInterval) {
      lastMqttPub = currentMillis;
      
      // Call the function from MqttHandler.cpp with the numeric values collected above
      // Publish sensor data to MQTT topics
      publishSensorData(valTemp, valHum, valIAQ, (float)iaqSensor.iaqAccuracy, valCO2, valTVOC, latestDustDensity);
  }
  // # User Interface: Button Handling
  if (buttonPressed) {
    buttonPressed = false; // Reset flag

    displayStartTime = currentMillis; // Reset Timeout Timestemp
    bool wasDisplayOff = !isDisplayOn;
    if(!isDisplayOn){
      // If display is off, wake it up and show Page 1
      isDisplayOn = true;
      currentPage = 1;
      turnDisplayOn();
      Serial.println("Wake Up -> Page 1");
    }
    else {
      // If display is already on, cycle to the next page
      currentPage++;
      if (currentPage > 4) {
          currentPage = 1;
      }
      Serial.print("Switching to Page "); Serial.println(currentPage);
    }
    // Render the specific page content immediately upon button press
    switch (currentPage) {
      case 1: // HTU21D
        displayLCDData(latestTempHTU, latestHumHTU, currentPage);
        break;
        
      case 2: // BME680
        displayLCDData(latestIAQ, latestAccuracy, currentPage);
        break;
        
      case 3: // SGP30 
        displayLCDData(latestCO2String, latestTVOCString, currentPage);
        break;
      case 4: // SharpDust
        displayLCDData(latestDustString, "PM2.5 Sensor", currentPage);
        break;
    }

    delay(200); // Debounce delay to prevent multiple triggers
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);
  }

  // # Power Management: Display Timeout
  // Turn off the display to save power after 15 seconds of inactivity
  if (isDisplayOn && (currentMillis - displayStartTime > 15000)) {
      isDisplayOn = false;
      turnDisplayOff();
      currentPage = 1; // Reset to Page 1 for next wake-up event
      Serial.println("Display Timeout: OFF");
      if (!isNetworkOnline()) {
          retryConnection();
      }
  }
}