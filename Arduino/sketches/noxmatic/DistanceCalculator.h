#include "Settings.h"
#include "Information.h"

#define SIGNAL_LOST_INTERVAL_MILLIS 600000;
#define SPEED_INTERVAL 500;

class DistanceCalculator {
public:
  DistanceCalculator(ChainOiler *chainOiler, Information *information, Settings *settings) {
    this->information = information;
    this->chainOiler = chainOiler;
    this->setSignalLost(false);
    long rotationLength = settings->getOilerRotationLength();
    long tickPerRotation = settings->getOilerTickPerRotation();
    if (tickPerRotation != 0) {
      this->speedTickFactor = rotationLength * 3600 / tickPerRotation;
      this->distancePerTick = (float)rotationLength / tickPerRotation;
    }
  }

  ~DistanceCalculator() {
  }

  void processTick() {
    speedTicks++;
    lastTickMillis = millis();
  }

  void process() {
    unsigned long currentMillis = millis();
    unsigned long signalLostMillis = lastTickMillis + SIGNAL_LOST_INTERVAL_MILLIS;
    if (currentMillis > signalLostMillis) {
      setSignalLost(true);
      //NOTLAUF
    } else {
      setSignalLost(false);
    }
    calculateSpeed();
  }

private:
  int currentSpeed = 0;
  int lastSpeed = 0;
  int speedTicks = 0;
  unsigned long lastCalcMillis = 0;
  unsigned long lastTickMillis = 0;
  unsigned long nextSpeedMillis = millis() + SPEED_INTERVAL;
  long speedTickFactor = 0;
  float distancePerTick = 0;
  Information *information;
  ChainOiler *chainOiler;

  void setSignalLost(bool lost) {
    information->speedSignalLost = lost;
  }

  void calculateSpeed() {
  
    unsigned long currentMillis = millis();
    if (currentMillis > nextSpeedMillis) {
      nextSpeedMillis = currentMillis + SPEED_INTERVAL;
  
      long currentTicks = (long)speedTicks;
      unsigned long lastTick = lastTickMillis;
      long relevantMillis = lastTick - lastCalcMillis;
      lastCalcMillis = lastTick;
      
      long calcSpeed = 0;
      if (relevantMillis != 0) {
        calcSpeed = (currentTicks * speedTickFactor) / relevantMillis;
      }
      calcSpeed /= 1000;
      currentSpeed = (calcSpeed + lastSpeed) / 2;
      information->speed = currentSpeed;

      long distance = currentTicks * distancePerTick;
      chainOiler->processDistance(distance);
  
      speedTicks -= currentTicks;
      lastSpeed = calcSpeed;
    }
  }
};
