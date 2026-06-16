/*
  Master Thesis - Air Quality Monitoring System
  Student: Mihai Sebastian-Andrei
    LCD Display Handling Module: LCD.cpp
    - Manages LCD initialization and data display
    - displayLCDData() function to show sensor data on LCD
    - power management functions to turn display on/off
 */


#include "LCD.h"

// Initialize the LCD object with I2C address 0x27, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * Performs the hardware initialization sequence for the LCD.
 * Sets the cursor, turns on the backlight, displays a boot message,
 * and then clears the screen to prepare for data.
 */

void setupLCD() {
    lcd.init(); // Initialize the LCD
    lcd.backlight(); // Turn on the backlight
   
    lcd.setCursor(0, 0);
    lcd.print("System start..."); // Display boot message
   
    delay(2000); // Delay to keep the boot message for 2 seconds
    lcd.clear(); // Clear the display after the boot message
    lcd.noBacklight(); // Turn off backlight to save power
}

/*
    * Displays two lines of text on the LCD along with the current page number.
    * Turns on the backlight and clears the display before showing new content.
    *
    * Parameters:
    *   labelUp - The text to display on the first line.
    *   labelDown - The text to display on the second line.
    *   nrPage - The current page number to display.
*/
void displayLCDData(String labelUp, String labelDown,int nrPage) {
    
        lcd.backlight();
        lcd.clear();
        
        // Display the first line
        lcd.setCursor(0, 0);
        lcd.print(labelUp); // Display the first line

        // Display the second line with page number
        lcd.setCursor(0, 1); 
        lcd.print(labelDown);lcd.print(" | "); lcd.print("P:");lcd.print(nrPage); 
        
}

void turnDisplayOff() {
    lcd.clear();
    lcd.noBacklight();
    lcd.noDisplay(); // Disables the LCD driver to save power
}

void turnDisplayOn() {
    lcd.backlight();
    lcd.display(); // Enables the LCD driver
}