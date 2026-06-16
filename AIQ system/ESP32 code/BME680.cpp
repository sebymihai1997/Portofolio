/*
  Master Thesis - Air Quality Monitoring System
  Student: Mihai Sebastian-Andrei
  
  BME680 Sensor Handling Module: BME680.cpp
  - Setup and interface with BME680 sensor
  - Functions to interface with BME680 sensor over I2C/SPI
  - Initialize BSEC library
  - Read IAQ values: IAQ Score, eCO2, TVOC
*/

#include "BME680.h"
#include "bsec.h"
#include <Wire.h>

// Create the BSEC object
Bsec iaqSensor;
String output;

/*
  This function checks the status of the BSEC library and the BME680 sensor.
  If there are any errors or warnings, it prints them to the Serial Monitor.
*/
void checkIaqSensorStatus(void)
{
  // Check for BSEC library status
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }
  // Check for BME680 sensor status
  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

// This function initializes the BME680 sensor and the BSEC library
void setupBME680()
{
  // Initialize I2C connection.
  // I2C Address: Try 0x77 first. If it fails, try 0x76.
  iaqSensor.begin(0x77, Wire); 
  
  // Print BSEC library version
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);

  checkIaqSensorStatus();

  // Define the list of virtual sensors/outputs we want the library to calculate.
  // The BSEC algorithm fuses temperature, humidity, and gas resistance to derive IAQ.
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,            // IAQ Score (0-500)
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT, // eCO2 Estimate derived from VOCs
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  // Subscribe to the virtual sensors with a sample rate of 3 seconds (Low Power Mode)
  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
}

/* 
  This function reads data from the BME680 sensor, processes it through the BSEC library,
  and updates the global variables with the latest IAQ values.
  This function should be called periodically (e.g., every 3 seconds).
  It returns true if new data is available, false otherwise.
*/
bool readBME680Data()
{
  if (iaqSensor.run()) { // If data is valid
    Serial.println("--- [BME680 Bosch Data] ---");
    Serial.print("IAQ Score: "); Serial.println(iaqSensor.iaq);
    Serial.print("Accuracy:  "); Serial.println(iaqSensor.iaqAccuracy);
    Serial.println("---------------------------");
    return true; 
  } else { //Data not ready, or an error occurred
    checkIaqSensorStatus();
    return false; 
  }
}