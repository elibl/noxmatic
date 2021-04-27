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
    this->pumpImpulses = settings->getOilerPumpImpulses();
    this->pumpImpulsesLeft = 0;
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
    if (!information->pumpActive) { activatePumpPin(); }
    pumpImpulsesLeft = pumpImpulses - 1;
  }

private:
  Information *information;
  int pinPump;
  int pumpImpulses;
  int pumpImpulsesLeft;
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
    if (!information->pumpActive && pumpImpulsesLeft > 0 && currentMillis - pumpDeactivateMillis > 100) {
      activatePumpPin();
      pumpImpulsesLeft--;
    }
  }
};
#endif
