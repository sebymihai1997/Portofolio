#ifndef SHARPDUST_H
#define SHARPDUST_H

#include <Arduino.h>

#define SHARP_LED_PIN 18
#define SHARP_VO_PIN 34


// Voltage Divider Ratio
// R_top = 2.2k (R2), R_bottom = 3.3k (R3)
// Factor = (R2 + R3) / R3 = (2.2 + 3.3) / 3.3 = 1.6666
#define VOLTAGE_DIVIDER_RATIO 1.6667 // 5V to 3.3V divider ratio

extern float latestDustDensity;

void setupSharpDust();
void readSharpDust();

#endif