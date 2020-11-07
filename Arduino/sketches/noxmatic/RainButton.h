#include "Information.h"

#ifndef RAINBUTTON_H_
#define RAINBUTTON_H_

#define RAINBUTTON_INTERVAL 50

class RainButton {
public:
  RainButton(int pinRainButton, Information *information, Pump *pump) {
    this->information = information;
    this->pump = pump;
    this->pinRainButton = pinRainButton;
  }

  ~RainButton() {
  }

  void init() {
    pinMode(pinRainButton, INPUT_PULLUP);
    buttonState = digitalRead(pinRainButton);
    information->rain = !buttonState;
  }

  void process() {
    processRainButton();
  }

private:
  Information *information;
  Pump *pump;
  int buttonState;
  int lastButtonState;
  int pinRainButton;
  unsigned long currentMillis = 0;
  unsigned long nextCheckMillis = 0;
  unsigned long pressedTimeMillis  = 0;

  void processRainButton() {
    currentMillis  = millis();
    if (currentMillis > nextCheckMillis) {
      nextCheckMillis = currentMillis + RAINBUTTON_INTERVAL;
      lastButtonState = buttonState;
      buttonState = digitalRead(pinRainButton);
      
      if (lastButtonState == HIGH && buttonState == LOW) pressedTimeMillis = currentMillis;
      else if (buttonState == LOW && currentMillis - pressedTimeMillis > 4000) pump->pump();
      else if (lastButtonState == LOW && buttonState == HIGH && currentMillis - pressedTimeMillis < 4000) {
        information->rain = !information->rain;
      }
    }
  }
};
#endif
