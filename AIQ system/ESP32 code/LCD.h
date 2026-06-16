#ifndef LCD_H
#define LCD_H   

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

void setupLCD();
void displayLCDData(String labelLeft, String labelRight,int nrPage);

// Simple Hardware Control
void turnDisplayOn();
void turnDisplayOff();


#endif