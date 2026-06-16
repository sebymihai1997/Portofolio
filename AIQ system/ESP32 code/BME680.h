#ifndef BME680_H
#define BME680_H


#include <Arduino.h> // Used for uint8_t, uint16_t, etc.
#include "bsec.h"

extern Bsec iaqSensor;

void checkIaqSensorStatus(void);
void setupBME680();
bool readBME680Data();


#endif