#include <Arduino.h>
#include <LedControl.h>

// Pins: DIN, CLK, LOAD (CS), number of devices
const int clkPin  = 6;
const int loadPin = 7;
const int dataPin = 8;

const int irPin = 12; // Pin for IR button input


void updateCounter();
void displayInteger(int value, int mostSignificantDigit);
void updateTimer();
void numberDigits();
void turnOnAllSegments();
void sequenceDPs();

LedControl* lc;

void setup() {
  pinMode(13, OUTPUT); // Onboard LED for debugging  
  digitalWrite(13, LOW); // Turn LED on

  pinMode(irPin, INPUT); // IR pin for button input

  lc = new LedControl(dataPin, clkPin, loadPin, 1); // Update if your pins are different

  lc->shutdown(0, false);       // Wake up from shutdown
  lc->setScanLimit(0, 8);       
  lc->setIntensity(0, 2);       // Set brightness to medium
  lc->clearDisplay(0);          // Clear display
}

int number = 0;
int counter = 0;

void loop() {
  // digitalRead(irPin); // Read the IR pin to simulate button press
  // if (digitalRead(irPin) == HIGH) {
  //   // Simulate button press
  //   updateCounter();
  //   digitalWrite(13, LOW); // Turn on onboard LED when button is pressed
  // } else {
  //   digitalWrite(13, HIGH); // Turn off onboard LED when button is not pressed
  // }
  //updateCounter();
  numberDigits();
  //turnOnAllSegments();
  sequenceDPs();
}

void numberDigits() {
  for (int digit = 0; digit < 8; digit++) {
    if (number == digit) {
      lc->setDigit(0, digit, '0', true);
      lc->setRow(0, digit, 0xFF);
    } else {
      lc->setChar(0, digit, '0' + digit, false);
    }
  }
}

void sequenceDPs() {
  delay(250);
  number += 1; 
  if (number > 8) {
    number = 0; 
  }
}

void turnOnAllSegments() {
  for (int digit = 0; digit < 8; digit++) {
    lc->setRow(0, digit, 0xFF); // Turn on all 8 segments for this digit
  }
}

void updateCounter() {
  counter++;
  if (counter > 9) {
    counter = 0; // Reset counter after 9999
  }

  delay(250);
  for (int digit = 0; digit < 8; digit++) {
    lc->setChar(0, digit, '0' + counter, false);
  }
}

void displayInteger(int value, int mostSignificantDigit) {
  int hundreds = (value / 100) % 10;
  int tens = (value / 10) % 10;
  int ones = value % 10;

  
  // Show digits on the display (right to left: position 2 = rightmost)
  if (value < 100 ) {
    lc->setChar(0, mostSignificantDigit, ' ', false); // Clear hundreds place if zero
  } else {  
    lc->setChar(0, mostSignificantDigit, '0' + hundreds, false);
  }

  if (value < 10) {
    lc->setChar(0, mostSignificantDigit+1, ' ', false);
  } else {
    lc->setChar(0, mostSignificantDigit+1, '0' + tens, false);
  }

  lc->setChar(0, mostSignificantDigit+2, '0' + ones, false);
}


void updateTimer() {
  // int thousands = (number / 1000) % 10;
  // int hundreds = (number / 100) % 10;
  // int tens = (number / 10) % 10;
  // int ones = number % 10;

  // if (thousands < 1000 ) {
  //   lc->setChar(0, 2, ' ', false); // Clear hundreds place if zero
  // } else {  
  //   lc->setChar(0, 2, '0' + hundreds, false);
  // }

  // // Show digits on the display (right to left: position 2 = rightmost)
  // if (hundreds < 100 ) {
  //   lc->setChar(0, 3, ' ', false); // Clear hundreds place if zero
  // } else {  
  //   lc->setChar(0, 3, '0' + hundreds, false);
  // }

  // if (number < 10) {
  //   lc->setChar(0, 4, ' ', false);
  // } else {
  //   lc->setChar(0, 4, '0' + tens, false);
  // }

  // lc->setChar(0, 5, '0' + ones, false);

  displayInteger(number, 0); // Display the number on the LED display
  displayInteger(number, 3); // Display the number on the LED display
  delay(50);
  number += 1;
  if (number > 9999) {
    number = 0; // Reset number after 999
  }
}

