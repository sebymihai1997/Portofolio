#ifndef HTU21D_H
#define HTU21D_H

#include <Arduino.h> // Used for uint16_t variables

// Function declaration
void softReset();
uint16_t getTemperatureRaw();
float calculateTemperature(uint16_t rawValue);
uint16_t getHumidityRaw();
float calculateHumidity(uint16_t rawValue);

#endif