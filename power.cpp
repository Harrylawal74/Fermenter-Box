#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "HX711.h" // Library for analog HX711


#define POWER_BUTTON_PIN *********
bool systemPower = FALSE;

void setup() {

  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP); //power button

}

void loop() {
 
}


void onOff(){
  if (digitalRead(POWER_BUTTON_PIN) == LOW) {

    delay(2000); // delay I/O button press for 5 seconds
    if (digitalRead(POWER_BUTTON_PIN) == LOW) {

      if (systenPower == TRUE) {
        systemPower = FALSE;
      } 
      else {
        systemPower = TRUE;
      }
    }
  }

  return systemPower;
}
