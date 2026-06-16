/*  
    Master Thesis - Air Quality Monitoring System
    Student: Mihai Sebastian-Andrei
    SharpDust.cpp
    Functions to interface with Sharp GP2Y1010AU0F Dust Sensor
    - Setup and read dust density values
    - Uses analog read and timing sequence as per datasheet
    - Calculates dust density in mg/m3
    - Returns the latest dust density value
*/
#include "SharpDust.h"

float latestDustDensity = 0.0;

void setupSharpDust() {

    pinMode(SHARP_LED_PIN, OUTPUT);
    pinMode(SHARP_VO_PIN, INPUT);
    // Ensure the infrared LED is OFF initially
    digitalWrite(SHARP_LED_PIN, LOW);
}

// This function performs a complete measurement cycle for Sharp GP2Y1010AU0F sensor
void readSharpDust() {
    // 1. Turn LED ON
    digitalWrite(SHARP_LED_PIN, HIGH); 
    
    // 2. Wait 280us (Sampling Time from Datasheet)
    delayMicroseconds(280); 
    
    // 3. Read the "Photo-diode" (Analog Value)
    int voRaw = analogRead(SHARP_VO_PIN); 
    
    // 4. Wait 40us (Pulse Width remainder to reach 320us total)
    delayMicroseconds(40); 
    
    // 5. Turn LED OFF
    digitalWrite(SHARP_LED_PIN, LOW); 
    
    //  CALCULATIONS 
    
    // Convert Raw ADC (0-4095 on ESP32) to Voltage (0-3.3V)
    // ESP32 ADC is not perfectly linear, but this is close enough
    float voltage = voRaw * (3.3 / 4095.0);
    
    // Compensate for Voltage Divider
    // Calculate the actual sensor voltage before the divider
    float voltageSensor = voltage * VOLTAGE_DIVIDER_RATIO;

    // Calculate dust density (mg/m3) using Datasheet Formula (Linear Approximation)
    // Dust Density (mg/m3) = 0.17 * Voltage - 0.1
    // 0.1 mg/m3 is the offset for zero dust concentration
    float dust = (0.17 * voltageSensor) - 0.1;
    
    // Sanity check: Dust cannot be negative
    if (dust < 0) {
        dust = 0.0;
    }
    
    latestDustDensity = dust;
    
    // Debugging output
    Serial.print("Dust Volts: "); Serial.print(voltage);
    Serial.print(" Density: "); Serial.println(latestDustDensity);
}