#include "Settings.h"
#include "Information.h"
#include "Pump.h"

#ifndef OILCALCULATOR_H_
#define OILCALCULATOR_H_

class ChainOiler {
public:
  ChainOiler(Information *information, Pump *pump, Settings *settings) {
    this->information = information;
    this->pump = pump;
    this->setPumpPending(false);
    this->emergencyPumpInterval = (long)settings->getOilerEmergencyInterval() * 1000;
    this->nextEmergencyPump = millis() + emergencyPumpInterval;
    this->requiredOilDistance = (long)settings->getOilerDistance() * 1000;
    this->remainingOilDistance = requiredOilDistance /2;
    this->lastRain = information->rain;
  }

  ~ChainOiler() {
  }

  void processDistance(long distance) {
    remainingOilDistance -= distance;
    if (remainingOilDistance < 1) {
      remainingOilDistance += requiredOilDistance;
      setPumpPending(true);
    }
    information->distancePercent = getDistancePercent();    
  }

  void process() {
    bool rain = information->rain;

    if (lastRain != rain) {
      if (rain) {
        requiredOilDistance = requiredOilDistance / 10;
        remainingOilDistance = remainingOilDistance / 10;
        emergencyPumpInterval = emergencyPumpInterval / 10;
        nextEmergencyPump -= emergencyPumpInterval * 9;
      }
      else {
        requiredOilDistance = requiredOilDistance * 10;
        remainingOilDistance = remainingOilDistance * 10;
        nextEmergencyPump += emergencyPumpInterval * 9;
        emergencyPumpInterval = emergencyPumpInterval * 10;
      }
    }
    lastRain = rain;
    
    unsigned long currentMillis = millis();
    if (information->speedSignalLost && currentMillis > nextEmergencyPump) {
      nextEmergencyPump = currentMillis + emergencyPumpInterval;
      pump->pump();
    }
    processSpeedPump();
  }

private:
  long requiredOilDistance;
  long remainingOilDistance;
	Pump *pump;
  bool lastRain;
	bool pumpPending;
	long emergencyPumpInterval;
  long nextEmergencyPump;
  Information *information;

  int getDistancePercent() {
    int percent = 0;
    if(requiredOilDistance > 0 && remainingOilDistance > 0) {
      unsigned long tmp = (requiredOilDistance - remainingOilDistance)* 100;
      percent = tmp / requiredOilDistance;
    }
    return percent;
  }

  void setPumpPending(bool pending) {
    pumpPending = pending;
    information->pumpPending = pumpPending;
  }

  void processSpeedPump() {
    if (pumpPending) {
      int currentSpeed = information->speed;
      if ((10 < currentSpeed && currentSpeed < 60) || getDistancePercent() > 20) {
        setPumpPending(false);
        pump->pump();
      }
    }
  }
};
#endif
