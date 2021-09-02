#include "Settings.h"
#include "Information.h"

#define SIGNAL_LOST_INTERVAL_MILLIS 600000;
#define SPEED_INTERVAL 500;

class DistanceCalculator {
public:
  DistanceCalculator(ChainOiler *chainOiler, Information *information, Settings *settings) {
    this->chainOiler = chainOiler;
    this->information = information;
    this->settings = settings;
    this->setSignalLost(false);
    long rotationLength = settings->getOilerRotationLength();
    long tickPerRotation = settings->getOilerTickPerRotation();
    float transmission = (float)settings->getOilerSprocketTeeth() / settings->getOilerRearSprocketTeeth();
    if (tickPerRotation != 0) {
      this->speedTickFactor = rotationLength * 3600 / tickPerRotation;
      this->distancePerTick = (float)rotationLength / tickPerRotation;
      this->speedTickFactor *= transmission;
      this->distancePerTick *= transmission;
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
  unsigned int lastDistanceSave = 0;
  int currentSpeed = 0;
  int lastSpeed = 0;
  int speedTicks = 0;
  unsigned long lastCalcMillis = 0;
  unsigned long lastTickMillis = 0;
  unsigned long nextSpeedMillis = millis() + SPEED_INTERVAL;
  long speedTickFactor = 0;
  float distancePerTick = 0;
  ChainOiler *chainOiler;
  Information *information;
  Settings *settings;

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
      
      if (relevantMillis != 0) {
        currentSpeed = (currentTicks * speedTickFactor) / relevantMillis;
      } else currentSpeed = 0;
      information->speed = currentSpeed / pow(10,3);
      if (information->ticks < pow(2,32)) { information->ticks += currentTicks; }

      long distance = currentTicks * distancePerTick;
      chainOiler->processDistance(distance);
      information->distance += distance;

      unsigned int distanceSinceSave = (information->distance - lastDistanceSave) * pow(10,-3);
      if(currentSpeed == 0 && lastSpeed > 0 && distanceSinceSave > 50) {
        settings->setTotalDistance(settings->getTotalDistance() + distanceSinceSave);
        settings->persist();
        lastDistanceSave = information->distance;
      }
  
      speedTicks -= currentTicks;
      lastSpeed = currentSpeed;
    }
  }
};
