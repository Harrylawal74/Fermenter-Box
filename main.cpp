#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "HX711.h" // Library for analog HX711


#define POWER_BUTTON_PIN 6      // Pin for power button
#define PH_SENSOR_PIN A0        // Analog pin connected to pH sensor
#define VOLTAGE_REF 5.0         // Reference voltage for ADC
#define ADC_RESOLUTION 1024.0   // 10-bit ADC resolution
#define DHTPIN 10               // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11           // DHT11 sensor type
#define HX711_DT_PIN 4          // Data pin for HX711
#define HX711_SCK_PIN 5         // Clock pin for HX711
#define PUMP_PIN 9              // Pin to control the pump
#define TARE_BUTTON_PIN 7       // Pin for tare button
#define TIMER_BUTTON_PIN 8      // Pin for starting the day counter


//power I/O variable
bool systemPower = false;


// Calibration Constants
#define CALIBRATION_OFFSET 0.00 // Adjust based on calibration
#define HX711_CALIBRATION_FACTOR 212.389541 // Corrected calibration factor

// Timer Variables
unsigned long lastPumpActivation = 0;  // Track last pump activation
const unsigned long PUMP_INTERVAL = 86400000; // 24 hours in milliseconds
const unsigned long PUMP_DURATION = 5000;  // Pump runs for 5 seconds
bool pumpRunning = false; // Flag to track pump state

// Stabilization Variables
const float MINIMUM_WEIGHT = 2.0; // Minimum detectable weight threshold

// Day Counter Variables
unsigned long timerStart = 0; // Stores the start time of the timer
bool timerRunning = false; // Flag to track timer state

// Initialize Sensors
DHT dht(DHTPIN, DHTTYPE);
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Function to calculate pH from voltage
float calculatePH(float voltage) {
  float sensitivity = 3.5;
  return 7.0 + ((2.5 - voltage) / sensitivity);
}


void setup() {
  Serial.begin(9600);

  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP); //power button

 
  lcd.noBacklight();
  lcd.clear();
  Serial.println("Device is powered off. Press button to power on.");




  //--------------------------------------------------------

    // DHT Sensor Setup
  dht.begin();

  // HX711 Setup
  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);
  scale.set_scale(HX711_CALIBRATION_FACTOR); // Set the correct calibration factor after taring

  // Pump Setup
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Ensure pump is off initially
 
  // Button Setup
  pinMode(TARE_BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(TIMER_BUTTON_PIN, INPUT_PULLUP); // Timer button

  //--------------------------------------------------------------
  scale.tare();
 
}

void loop() {
  onOff();
  if (systemPower == true){
    readSensors();

    // Calculate Salt Amount (2% of weight, rounded to nearest 0.5g)
    float saltAmount = ceil((weight * 0.02) / 0.5) * 0.5;

    // Calculate Days Since Timer Start
    unsigned long elapsedMillis = millis() - timerStart;
    int elapsedDays = timerRunning ? elapsedMillis / 86400000 : 0; // Convert milliseconds to days
   

    //Seconds
    //int elapsedDays = timerRunning ? elapsedMillis / 1000 : 0; // Convert milliseconds to seconds


    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("pH: "); lcd.print(pHValue, 2);
    lcd.setCursor(11, 0);
    lcd.print("Days: "); lcd.print(elapsedDays);
   
    lcd.setCursor(0, 1);
    lcd.print("Weight: "); lcd.print(weight, 1); lcd.print(" g");
    lcd.setCursor(0, 2);
    lcd.print("Add Salt: "); lcd.print(saltAmount, 1); lcd.print(" g");
    lcd.setCursor(0, 3);
    lcd.print("Temp: "); lcd.print(temperature, 1); lcd.print(" C");

  }
}


void onOff(){
  if (digitalRead(POWER_BUTTON_PIN) == LOW) {

    delay(1500); // delay I/O button press for 5 seconds
    if (digitalRead(POWER_BUTTON_PIN) == LOW) {
       
      if (systemPower == true) {
        systemPower = false;
        Serial.println("Device Powered Off");
        lcd.noBacklight();
        lcd.clear();
      }
      else {
        systemPower = true;
        Serial.println("Device Powered On");
        lcd.init();
        lcd.backlight();
        lcd.setCursor(5, 0);
        lcd.print("Fervere");
        lcd.setCursor(4, 1);
        lcd.print("Streamlined");
        lcd.setCursor(4, 2);
        lcd.print("Fermenting");
        loading();
        lcd.clear();
      }
    }
  }

  return systemPower;
}




void tareButton(){
  // Handle Tare Button
  if (digitalRead(TARE_BUTTON_PIN) == LOW) {
    delay(200); // Debounce delay
    if (digitalRead(TARE_BUTTON_PIN) == LOW) { // Confirm still pressed
      Serial.println("Tare Button Pressed, resetting scale...");
      scale.tare(); // Reset scale to zero
      Serial.println("Scale tared successfully.");
    }
  }
}


void timerButton(){
  if (digitalRead(TIMER_BUTTON_PIN) == LOW) {
    delay(200); // Debounce delay
    if (digitalRead(TIMER_BUTTON_PIN) == LOW) { // Confirm still pressed
      if (!timerRunning) {
        Serial.println("Timer Button Pressed, starting day counter...");
        timerStart = millis(); // Start timer
        timerRunning = true;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Ferment Started");
        lcd.setCursor(0, 2);
        lcd.print("Happy Fermenting!");
        delay(1500);
      } else {
        Serial.println("Timer Button Pressed, stopping day counter...");
        timerRunning = false;
        timerStart = 0; // Reset timer
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Ferment Stopped");
        delay(1500);
      }
    }
  }
}

void loading(){
  for (int i = 0; i < 2; i++){
    lcd.setCursor(9, 3);
    lcd.print(".");
    delay(333);
   
    lcd.setCursor(10, 3);
    lcd.print(".");
    delay(333);
   
    lcd.setCursor(11, 3);
    lcd.print(".");
    delay(333);


    lcd.setCursor(9, 3);
    lcd.print(" ");
    lcd.setCursor(10, 3);
    lcd.print(" ");
    lcd.setCursor(11, 3);
    lcd.print(" ");
    delay(200); 
   
  }
  lcd.clear();
}


void readSensors(){
    // Read pH Sensor
    int analogValue = analogRead(PH_SENSOR_PIN);
    float voltage = (analogValue / ADC_RESOLUTION) * VOLTAGE_REF;
    float pHValue = calculatePH(voltage) + CALIBRATION_OFFSET;

    // Read Temperature from DHT11
    float temperature = dht.readTemperature();


    tareButton();
    timerButton();

    // Read Weight from HX711 and apply minimum threshold
    float weight = scale.get_units(10); // Get current weight
    if (weight < MINIMUM_WEIGHT) {
      weight = 0.0; // Ignore small weights
    }
}
