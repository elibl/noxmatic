#include "Information.h"
#include "Settings.h"

#ifndef PUMP_H_
#define PUMP_H_

class Pump {
public:
  Pump(int pinPump, Information *information, Settings *settings) {
    this->information = information;
    this->settings = settings;
    this->pinPump = pinPump;
    this->pumpImpulses = settings->getOilerPumpImpulses();
    this->pumpImpulsesLeft = 0;
    this->pumpImpulseVolume = settings->getOilerPumpImpulseVolume();
    this->pumpDeactivateMillis = 0;
    this->pumpDurationMillis = (long)settings->getOilerPumpDuration();
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
    if (!information->pumpActive) activatePumpPin();
    pumpImpulsesLeft = pumpImpulses - 1;
  }

private:
  Information *information;
  Settings *settings;
  int pinPump;
  int pumpImpulses;
  int pumpImpulsesLeft;
  int pumpImpulseVolume;
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
    information->oilLevelPercent -= pumpImpulseVolume * pow(10,-4) / settings->getOilerReservoir();
  }

  void processPump() {
    unsigned long currentMillis = millis();

    if (information->pumpActive && pumpDeactivateMillis < currentMillis) deactivatePumpPin();
    if (!information->pumpActive && pumpImpulsesLeft > 0 && currentMillis - pumpDeactivateMillis > 100) {
      activatePumpPin();
      pumpImpulsesLeft--;
      if (pumpImpulsesLeft == 0) {
      settings->setOilerLevel(information->oilLevelPercent);
      settings->persist();
      }
    }
  }
};
#endif
