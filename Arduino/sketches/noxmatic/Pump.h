#include "Settings.h"

#ifndef PUMP_H_
#define PUMP_H_

class Pump {
public:
  Pump(int pinPump, Settings *settings) {
    pumpActive = false;
    this->pinPump = pinPump;
    pinMode(pinPump, OUTPUT);
    pumpDeactivateMillis = 0;
    pumpDurationMillis = settings->getOilerPumpDuration();
  }

  ~Pump() {
  }

  void init() {
    deactivatePumpPin();
  }

  void process() {
    processPump();
  }

  void pump() {
    activatePumpPin();
  }

private:
  int pinPump;
  bool pumpActive;
  long pumpDeactivateMillis;
  long pumpDurationMillis;

  void deactivatePumpPin() {
    pumpActive = false;
    digitalWrite(pinPump, LOW);
  }
  
  void activatePumpPin() {
    pumpActive = true;
    digitalWrite(pinPump, HIGH);
    pumpDeactivateMillis = millis() + pumpDurationMillis;
  }

  void processPump() {
    unsigned long currentMillis = millis();
    if (pumpActive && pumpDeactivateMillis < currentMillis) {
      deactivatePumpPin();
    }
  }
};
#endif
