#include <Arduino.h>

// Declarațiile funcțiilor
void stopCharging();
void finalizeLoading();
void updateChargingStatus();

const int startButtonPin = 2;  // pint pt start
const int stopButtonPin = 3;   // Pin de stop
const int ledPins[] = {10, 9, 8, 7}; // Pin pt leduri
const int redPin = 6;    // Pin pt ocupat
const int greenPin = 4;  // Pin for liber

enum State { LIBER, INCARCARE };
State currentState = LIBER;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long longPressTime = 1000;
unsigned long lastBlinkTime = 0; 
unsigned long lastChargingTime = 0; 
int chargeLevel = 0;
bool charging = false;
bool lastStartButtonState = LOW;
bool lastStopButtonState = LOW;
bool blinkState = false; // For LED blinking

void setup() {
  pinMode(startButtonPin, INPUT_PULLUP); 
  pinMode(stopButtonPin, INPUT_PULLUP);  
  
  // Initialize loading LEDs
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // incep cu toate oprite
  }

  // initializez starea ocupat
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  //incep cu ea in liber
  digitalWrite(greenPin, HIGH);  // verde pt liber
  digitalWrite(redPin, LOW);     // rosu pt ocupat
}

void loop() {
  int startButtonState = digitalRead(startButtonPin);
  int stopButtonState = digitalRead(stopButtonPin);
  unsigned long currentTime = millis();

  // Handle start button
  if (startButtonState != lastStartButtonState) {
    lastDebounceTime = currentTime;
  }
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    if (startButtonState == LOW && currentState == LIBER) {
      currentState = INCARCARE;
      digitalWrite(greenPin, LOW);  // opreste verde
      digitalWrite(redPin, HIGH);   // porneste rosu
      lastChargingTime = millis();  // incepe sa numere
      chargeLevel = 0; // reseteaza 
      charging = true;
    }
  }
  lastStartButtonState = startButtonState;

  // butonul de stop
  if (stopButtonState != lastStopButtonState) {
    lastDebounceTime = currentTime;
  }
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    if (stopButtonState == LOW && currentState == INCARCARE) { 
      unsigned long pressDuration = 0;
      while (digitalRead(stopButtonPin) == LOW) { 
        pressDuration = millis() - currentTime;
        if (pressDuration > longPressTime) { //daca este apasat butonul
          stopCharging(); //opreste incarcarea
          return;
        }
      }
    }
  }
  lastStopButtonState = stopButtonState;

  // procesul de incarcare
  if (charging) {
    // punem 3 secunde
    if (currentTime - lastChargingTime >= 3000) {
      lastChargingTime = currentTime; // Resetam dupa
      chargeLevel++;
      if (chargeLevel > 4) {
        finalizeLoading(); 
      }
    }

    // clipire led-uri
    if (currentTime - lastBlinkTime >= 500) {
      lastBlinkTime = currentTime;
      blinkState = !blinkState; 
      updateChargingStatus();
    }
  }
}

void updateChargingStatus() {
  // stinge toate luminile
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  // pastreaza le pe cele deja aprinse(pt a simula incarcarea)
  for (int i = 0; i < chargeLevel - 1; i++) {
    digitalWrite(ledPins[i], HIGH);
  }

  // incarcarea ledului prezent
  if (chargeLevel > 0 && chargeLevel <= 4) {
    digitalWrite(ledPins[chargeLevel - 1], blinkState ? HIGH : LOW);
  }
}

void stopCharging() {
  charging = false;
  finalizeLoading(); 
}

void finalizeLoading() {
  for (int k = 0; k < 3; k++) { 
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH); 
    }
    delay(500); //
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], LOW); 
    }
    delay(500); //
  }

  digitalWrite(greenPin, HIGH);  
  digitalWrite(redPin, LOW);    
  currentState = LIBER;         
  charging = false;
  chargeLevel = 0;              
}
