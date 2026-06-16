/*
  Master Thesis - Air Quality Monitoring System
  Student: Mihai Sebastian-Andrei
  
  
  HTU21D Sensor Handling Module: HTU21D.cpp
  - Function to setup HTU21D sensor
  - Functions to interface with HTU21D sensor over I2C
  - Read temperature and humidity raw data
  - Convert raw data to metric units
 */
#include "HTU21D.h" 
#include <Wire.h>   

// Soft Reset function to deffault settings
void softReset() {
  Wire.beginTransmission(0x40); // Slave Address HTU21D
  Wire.write(0xFE);             // Soft Reset command 0xFE sent over I2C
  Wire.endTransmission();
  delay(15); // Wait for the reset to complete
}

/* 
  Function to read raw temperature data from HTU21D,
  returns a 16-bit unsigned integer representing the raw temperature value.
  Uses "No Hold Master Mode" for reading.
*/
uint16_t getTemperatureRaw() {
  Wire.beginTransmission(0x40); 
  Wire.write(0xF3); // Send "Measure Temperature, No Hold Master Mode" command 0xF3 over I2C 
  Wire.endTransmission();
  
  delay(55); // Maximum time for temperature measurement is 50ms and we add a small margin

  uint8_t bytesReceived = Wire.requestFrom(0x40, 3);  
  
  if (bytesReceived == 3) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    uint8_t crc = Wire.read(); // Read CRC byte checksum (clears the buffer)
    (void)crc; // Cast to void to avoid the warning about not using the CRC

    // Combine MSB and LSB into a single 16-bit value
    uint16_t rawValue = (msb << 8) | lsb; 

    // According to datasheet, the last two bits are status bits and should be cleared
    // We mask them out to get the actual raw temperature value.
    rawValue &= ~0x0003;
    
    return rawValue;
  }
  return 0; 
}

/*
  Function to convert raw temperature value to degrees Celsius.
  Takes a 16-bit unsigned integer as input and returns the temperature as a float.
  Formula from HTU21D datasheet:
  Temperature (°C) = -46.85 + 175.72 * (RawValue / 2^16)
*/
float calculateTemperature(uint16_t rawValue) {
  if (rawValue == 0) return 0.0; 
  
  float temperature = (float)rawValue;
  temperature *= 175.72;
  temperature /= 65536.0; // 2^16 = 65536
  temperature -= 46.85;
  
  return temperature;
}

/* 
  Function to read raw Humidity data from HTU21D,
  returns a 16-bit unsigned integer representing the raw humidity value.
  Uses "No Hold Master Mode" for reading.
*/
uint16_t getHumidityRaw() {
  Wire.beginTransmission(0x40);
  Wire.write(0xF5); // Send "Measure Humidity, No Hold Master Mode" command 0xF5 over I2C
  Wire.endTransmission();
  
  delay(20); // Maximum time for humidity measurement is 16ms and we add a small margin
  
  uint8_t bytesReceived = Wire.requestFrom(0x40, 3); 
  
  if (bytesReceived == 3) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    uint8_t crc = Wire.read(); // Read CRC byte checksum (clears the buffer)
    (void)crc; // Cast to void to avoid the warning about not using the CRC

    // Combine MSB and LSB into a single 16-bit value
    uint16_t rawValue = (msb << 8) | lsb;
    
    // According to datasheet, the last two bits are status bits and should be cleared
    // We mask them out to get the actual raw temperature value.
    rawValue &= ~0x0003; 
    
    return rawValue;
  }
  return 0;
}

/*
  Function to convert raw humidity value to relative humidity percentage.
  Takes a 16-bit unsigned integer as input and returns the humidity as a float.
  Formula from HTU21D datasheet:
  Humidity (%) = -6 + 125 * (RawValue / 2^16)
*/
float calculateHumidity(uint16_t rawValue) {
  if (rawValue == 0) return 0.0;
  
  float humidity = (float)rawValue;
  humidity *= 125.0;
  humidity /= 65536.0;
  humidity -= 6.0;
  
  // Constrain results to physical limits (0-100%)
  if (humidity > 100.0) humidity = 100.0;
  if (humidity < 0.0) humidity = 0.0;
  
  return humidity;
}