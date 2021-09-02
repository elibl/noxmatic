#include "Information.h"

#ifndef RAINBUTTON_H_
#define RAINBUTTON_H_

#define RAINBUTTON_INTERVAL 50

class RainButton {
public:
  RainButton(int pinRainButton, Information *information, Pump *pump, Settings *settings) {
    this->information = information;
    this->pump = pump;
    this->settings = settings;
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
  Settings *settings;
  int buttonState;
  int fastPress = 0;
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

      int pressDuration = currentMillis - pressedTimeMillis;
      
      if (lastButtonState == HIGH && buttonState == LOW) {
        if (pressDuration < 500) fastPress++;
        else fastPress = 0;
        pressedTimeMillis = currentMillis;
      }
      else if (fastPress > 3) {
        settings->resetOilerLevel();
        fastPress = 0;
      }
      else if (buttonState == LOW && pressDuration > 5000) pump->pump();
      else if (lastButtonState == LOW && buttonState == HIGH && pressDuration <= 4000) {
        information->rain = !information->rain;
      }
    }
  }
};
#endif
