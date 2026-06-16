/*
    Master Thesis - Air Quality Monitoring System
    Student: Mihai Sebastian-Andrei
    
    SGP30 Sensor Handling Module: SGP30.cpp

    - Functions to interface with SGP30 sensor over I2C
    - Initialize sensor and read CO2 and TVOC data

*/
#include "SGP30.h"
#include <Wire.h>

uint16_t latestCO2 = 0;
uint16_t latestTVOC = 0;

/* 
    Function to send a 16-bit command to the SGP30 sensor over I2C.
    The command is split into two bytes (MSB and LSB) and sent sequentially.
*/
void sendCommand(uint16_t command) {
    Wire.beginTransmission(SGP30_I2C_ADDRESS);
    Wire.write(command >> 8);   // Send MSB
    Wire.write(command & 0xFF); // Send LSB
    Wire.endTransmission();
}

/* 
    Function to initialize the SGP30 sensor.
    Sends the initialization command (0x2003) and waits for the sensor to be ready.
*/
void setupSGP30() {
    Wire.begin();
    sendCommand(SGP30_CMD_INIT);
    delay(10); // Wait for the sensor to initialize the internal logic
    Serial.println("SGP30 Initialized (0x2003 sent)");
}

/*
    This function calculates the CRC-8 checksum for the given two data bytes.
    SGP30 uses CRC-8 with polynomial 0x31 (x^8 + x^5 + x^4 + 1) for data integrity.
    
    Parameters:
    - data1: The first byte of data.
    - data2: The second byte of data.
    Returns:
    - The calculated CRC-8 checksum.
*/
uint8_t calculateCRC(uint8_t data1, uint8_t data2) {
    uint8_t crc = 0xFF;
    uint8_t data[2] = {data1, data2};
    for(int i = 0; i < 2; i++) {
        crc ^= data[i];
        for(int bit = 0; bit < 8; bit++) {
            if(crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/*
    Reads the CO2 and TVOC data from the SGP30 sensor.
    Performs CRC checks on the received data to ensure integrity.
    Returns true if data is read successfully, false otherwise.
*/
bool readSGP30Data() {
    sendCommand(SGP30_CMD_MEASURE);
    delay(50); // Delay for the measurement to complete

    // Request 6 Bytes: CO2 (2 bytes) + CRC (1 byte) + TVOC (2 bytes) + CRC (1 byte)
    Wire.requestFrom(SGP30_I2C_ADDRESS, 6);
    if (Wire.available() < 6) {
        Serial.println("Error: Not enough data received from SGP30");
        return false;
    }

    uint8_t data[6];
    for (int i = 0; i < 6; i++) {
        data[i] = Wire.read();
    }

    // Validate CRC for CO2
    // Bytes 0 and 1, check against Byte 2
    if (calculateCRC(data[0], data[1]) != data[2]) {
        Serial.println("Error: CRC mismatch for CO2 reading");
        return false;
    }

    // Validate CRC for TVOC
    // Bytes 3 and 4, check against Byte 5
    if (calculateCRC(data[3], data[4]) != data[5]) {
        Serial.println("Error: CRC mismatch for TVOC reading");
        return false;
    }
    // Combine bytes to form final 16-bit values
    // Assign to global variables
    latestCO2 = (data[0] << 8) | data[1];
    latestTVOC = (data[3] << 8) | data[4];

    return true;
}