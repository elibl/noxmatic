#include "Information.h"

#ifndef LED_H_
#define LED_H_

#define LED_INTERVAL 50;

class Led {
public:
  Led(int pinLed, Information *information) {
    this->information = information;
    this->pinLed = pinLed;
  }

  ~Led() {
  }

  void init() {
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, LOW);
    ledState = digitalRead(pinLed);
    }

  void process() {
    processLed();
  }

private:
  Information *information;
  int bootBlinks = 0;
  int ledState = 0;
  int pinLed;
  unsigned long currentMillis = 0;
  unsigned long nextCheckMillis = 0;
  unsigned long lastLedChange  = 0;

  void processLed() {
    
    currentMillis = millis();
    if (currentMillis > nextCheckMillis) {
      nextCheckMillis = currentMillis + LED_INTERVAL;
      if (bootBlinks < 4) {
        blink(1000);
        if (lastLedChange == currentMillis) bootBlinks++;
      }
      else if (information->pumpActive) {
      digitalWrite(pinLed, ledState = HIGH);
      lastLedChange = currentMillis;
      }
      else if (information->speedSignalLost) blink(250);
      else if (information->oilLevelPercent <= 0) digitalWrite(pinLed, ledState = HIGH);
      else if (information->oilLevelPercent < 20) blink(information->oilLevelPercent * pow(10,3));
      else if (information->rain) blink(1000);
      else if (ledState == HIGH) {
      digitalWrite(pinLed, ledState = LOW);
      lastLedChange = currentMillis;
      }
    }
  }
  
  void blink(int millis) {
    if (information->rain) millis /= 2;
    if (currentMillis > lastLedChange + millis) {
      digitalWrite(pinLed, ledState = ledState ? LOW : HIGH);
      lastLedChange = currentMillis;
    }
  }
};
#endif
