#ifndef SGP30_H
#define SGP30_H

#include <Arduino.h> // Used for uint8_t, uint16_t, etc.

#define SGP30_I2C_ADDRESS 0x58

#define SGP30_CMD_INIT 0x2003
#define SGP30_CMD_MEASURE 0x2008

extern uint16_t latestCO2;
extern uint16_t latestTVOC;

void setupSGP30();
bool readSGP30Data();
uint8_t calculateCRC(uint8_t data1, uint8_t data2);


#endif