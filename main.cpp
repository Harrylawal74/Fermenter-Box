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
const int tareButtonPin = 4;

//add button pin for weight percentage change button
const int weightPercentButtonPin = 5;

/*
//variable for reading button states
int onButtonState = 0;
int phButtonState = 0;
int tareButtonState = 0;
int weightPercentButtonState = 0;
*/

byte onButtonPrevState = 0;
bool onButtonReleased = false;
unsigned long onButtonPressedAt = 0;

void setup() {
  pinMode(onButtonPin, INPUT_PULLUP);

}

void loop() {
  onButton();
}



void onButton() {
  //reads the state of the on button (is it pushed down or not?)
  int onButtonState = digitalRead(onButtonPin);

  // if the button has changed state then checks whether it has been pressed and records the time it was pressed
  if (onButtonState != onButtonPrevState) {
    if (onButtonState == HIGH) {
      onButtonReleased = false;
      onButtonPressedAt = millis();
    }
    else {
      onButtonReleaseed = true;
    }
    onButtonPrevState = onButtonState;
  }

  if (onButtonReleased == true) {
    after5sec();
    onButtonReleased = false;
    onButtonPressedAt = 0;
  }
}





void after5sec() {
  //if the button is pressed for 5+ seconds then the box is turned on/off
  long diff = millis() - onButonPressedAt;
  if (diff > 5000) {
    if (onButtonState == LOW){
      digitalWrite(onButtonPin, HIGH);
    }
    else {
      digitalWrite(onButtonPin, LOW);
    }
  }

  if (diff < 2000) {
    unsigned long timeSinceStart = 0;

    
  }
}
