#include "Information.h"
#include "Settings.h"

#ifndef PUMP_H_
#define PUMP_H_

class Pump {
public:
  Pump(int pinPump, Information *information, Settings *settings) {
    this->information = information;
    this->pinPump = pinPump;
    this->pumpDeactivateMillis = 0;
    this->pumpDurationMillis = settings->getOilerPumpDuration();
  }

  ~Pump() {
  }

  void init() {
    pinMode(pinPump, OUTPUT);
    deactivatePumpPin();
  }

  void process() {
    processPump();
  }

  void pump() {
    activatePumpPin();
  }

private:
  Information *information;
  int pinPump;
  long pumpDeactivateMillis;
  long pumpDurationMillis;

  void deactivatePumpPin() {
    information->pumpActive = false;
    digitalWrite(pinPump, LOW);
  }
  
  void activatePumpPin() {
    information->pumpActive = true;
    digitalWrite(pinPump, HIGH);
    pumpDeactivateMillis = millis() + pumpDurationMillis;
  }

  void processPump() {
    unsigned long currentMillis = millis();
    if (information->pumpActive && pumpDeactivateMillis < currentMillis) {
      deactivatePumpPin();
    }
  }
};
#endif
