#include "ChainOiler.h"
#include "CommunicationESP.h"
#include "Display.h"
#include "DistanceCalculator.h"
#include "Heater.h"
#include "Information.h"
#include "Led.h"
#include "Pump.h"
#include "RainButton.h"
#include "Settings.h"
#include "TemperatureCalculator.h"


#define PIN_LED D0
#define PIN_RAINBUTTON D1
#define PIN_TEMPERATURE_DATA D2
#define PIN_HEATER1 D3
#define PIN_HEATER2 D4
#define PIN_SPEED_SIGNAL D6
#define PIN_PUMP D8

Settings settings;
Information information;
//TemperatureCalculator temperatureCalculator(PIN_TEMPERATURE_DATA, &information);
Led led(PIN_LED, &information);
Pump pump(PIN_PUMP, &information, &settings);
ChainOiler chainOiler(&information, &pump, &settings);
//Heater heater(PIN_HEATER1, PIN_HEATER2, &information, &settings);
RainButton rainButton(PIN_RAINBUTTON, &information, &pump);
//Display display(&information, &settings);
CommunicationESP communication(&information, &pump, &settings);
DistanceCalculator distanceCalculator(&chainOiler, &information, &settings);

void setup() {
  //Serial.begin(115200);
  led.init();
  pump.init();
  //heater.init();
  //temperatureCalculator.init();
  rainButton.init();
  communication.init();
  pinMode(PIN_SPEED_SIGNAL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SPEED_SIGNAL), speedSignalTrigger, FALLING);
}

void loop() {
  led.process();
  rainButton.process();
  //display.process();
  pump.process();
  communication.process();
  //temperatureCalculator.process();
  distanceCalculator.process();
  chainOiler.process();
  //heater.process();
}

ICACHE_RAM_ATTR void speedSignalTrigger() {
  distanceCalculator.processTick();
}
