#include <LiquidCrystal.h>
#include "HX711.h"
#include "<math.h>"

// set the LCD address to 0x64 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x64,20,4);


// add button pin for I/O button 
const int onButtonPin = 2;

// add button pin for pH sensor calibrator button 
const int phButtonPin = 3;

// add button pin for tare button 
const int tareButtonPin 4;

//add button pin for weight percentage change button
const int weightPercentButtonPin = 5;




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  if 

}
