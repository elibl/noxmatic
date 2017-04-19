// Do not remove the include below
#include "ChainOiler.h"
#include "Settings.h"
#include "Heater.h"
#include "Display.h"

#define PIN_SPEED_SIGNAL 2
#define PIN_PUMP 6

#define PIN_TEMPERATURE_DATA 4
#define PIN_HEATER 5

#define PIN_DISPLAY_SI 11
#define PIN_DISPLAY_CLK 13
#define PIN_DISPLAY_RS 8
#define PIN_DISPLAY_CSB 9

const unsigned long startUpTime = 10000;

bool setupMode = false;
bool startUp = true;

ChainOiler chainOiler(PIN_PUMP);
Heater heater(PIN_HEATER, PIN_TEMPERATURE_DATA);
Settings settings;
Display display(&heater, &chainOiler, &settings);

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SPEED_SIGNAL, INPUT);
  digitalWrite(PIN_SPEED_SIGNAL, HIGH);
  attachInterrupt(0, speedSignalTrigger, FALLING);
  initSettings();
  display.setSetupMode(true);
}

void initSettings() {
  chainOiler.init(&settings);
  heater.init(settings.getHeaterStartTemp(), settings.getHeaterStartPower(), settings.getHeaterMaxTemp(), settings.getHeaterMaxPower());
}

void loop() {
  chainOiler.process();
  heater.process();
  display.process();
  if (startUp) {
    if (millis() > startUpTime) {
      startUp = false;
      display.setSetupMode(false);
    }
  }
}

void speedSignalTrigger() {
  chainOiler.processTick();
}
