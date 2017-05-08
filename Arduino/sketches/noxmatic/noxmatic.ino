#include "Pump.h"
#include "ChainOiler.h"
#include "Settings.h"
#include "Heater.h"
#include "Display.h"
#include "CommunicationESP.h"
#include "Information.h"
#include "DistanceCalculator.h"
#include "TemperatureCalculator.h"

#define PIN_SPEED_SIGNAL D6
#define PIN_PUMP D8

#define PIN_TEMPERATURE_DATA D2
#define PIN_HEATER1 D3
#define PIN_HEATER2 D4

Settings settings;
Information information;
TemperatureCalculator temperatureCalculator(PIN_TEMPERATURE_DATA, &information);
Pump pump(PIN_PUMP);
ChainOiler chainOiler(&pump, &settings, &information);
Heater heater(PIN_HEATER1, PIN_HEATER2, &settings, &information);
Display display(&settings, &information);
CommunicationESP communication(&settings);
DistanceCalculator distanceCalculator(&settings, &information, &chainOiler);

void setup() {
  Serial.begin(115200);
  pump.init();
  heater.init();
  communication.connectWifi();
  pinMode(PIN_SPEED_SIGNAL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SPEED_SIGNAL), speedSignalTrigger, FALLING);
}

void loop() {
  temperatureCalculator.process();
  communication.process();
  distanceCalculator.process();
  chainOiler.process();
  heater.process();
  display.process();
  pump.process();
}

void speedSignalTrigger() {
  distanceCalculator.processTick();
}
