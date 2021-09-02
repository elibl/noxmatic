#include <EEPROM.h>
#include "Information.h"

#ifndef SETTINGS_H_
#define SETTINGS_H_

const int HEATER_BALANCE_ADDRESS = 0;
const int HEATER_MAXPOWER_ADDRESS = 4;
const int HEATER_MAXTEMP_ADDRESS = 8;
const int HEATER_STARTPOWER_ADDRESS = 16;
const int HEATER_STARTTEMP_ADDRESS = 20;

const int OILER_DISTANCE_ADDRESS = 24;
const int OILER_EMERGENCYINTERVAL_ADDRESS = 28;
const int OILER_LEVEL_ADDRESS = 32;
const int OILER_PUMPDURATION_ADDRESS = 36;
const int OILER_PUMPIMPULSES_ADDRESS = 40;
const int OILER_PUMPIMPULSEVOLUME_ADDRESS = 44;
const int OILER_REARSPROCKETTEETH_ADDRESS = 48;
const int OILER_RESERVOIR_ADDRESS = 52;
const int OILER_ROTATIONLENGTH_ADDRESS = 56;
const int OILER_SPROCKETTEETH_ADDRESS = 60;
const int OILER_TICKPERROTATION_ADDRESS = 64;

const int TOTALDISTANCE_ADDRESS = 68;

class Settings {

public:
  Settings(Information *information) {
    this->information = information;

    EEPROM.begin(512);
    EEPROM.get(HEATER_BALANCE_ADDRESS, heaterBalance);
    EEPROM.get(HEATER_MAXPOWER_ADDRESS, heaterMaxPower);
    EEPROM.get(HEATER_MAXTEMP_ADDRESS, heaterMaxTemp);
    EEPROM.get(HEATER_STARTPOWER_ADDRESS, heaterStartPower);
    EEPROM.get(HEATER_STARTTEMP_ADDRESS, heaterStartTemp);

    EEPROM.get(OILER_DISTANCE_ADDRESS, oilerDistance);
    EEPROM.get(OILER_EMERGENCYINTERVAL_ADDRESS, oilerEmergencyInterval);
    EEPROM.get(OILER_LEVEL_ADDRESS, oilerLevel);
    EEPROM.get(OILER_PUMPDURATION_ADDRESS, oilerPumpDuration);
    EEPROM.get(OILER_PUMPIMPULSES_ADDRESS, oilerPumpImpulses);
    EEPROM.get(OILER_PUMPIMPULSEVOLUME_ADDRESS, oilerPumpImpulseVolume);
    EEPROM.get(OILER_REARSPROCKETTEETH_ADDRESS, oilerRearSprocketTeeth);
    EEPROM.get(OILER_RESERVOIR_ADDRESS, oilerReservoir);
    EEPROM.get(OILER_ROTATIONLENGTH_ADDRESS, oilerRotationLength);
    EEPROM.get(OILER_SPROCKETTEETH_ADDRESS, oilerSprocketTeeth);
    EEPROM.get(OILER_TICKPERROTATION_ADDRESS, oilerTickPerRotation);

    EEPROM.get(TOTALDISTANCE_ADDRESS, totalDistance);

    this->information->oilLevelPercent = this->getOilerLevel();
    //this->setTotalDistance(27561 * pow(10,3)); //27264
    //this->persist();
  }
  
  ~Settings() {
  }
  
  void persist() {
    EEPROM.put(HEATER_BALANCE_ADDRESS, heaterBalance);
    EEPROM.put(HEATER_MAXPOWER_ADDRESS, heaterMaxPower);
    EEPROM.put(HEATER_MAXTEMP_ADDRESS, heaterMaxTemp);
    EEPROM.put(HEATER_STARTPOWER_ADDRESS, heaterStartPower);
    EEPROM.put(HEATER_STARTTEMP_ADDRESS, heaterStartTemp);

    EEPROM.put(OILER_DISTANCE_ADDRESS, oilerDistance);
    EEPROM.put(OILER_EMERGENCYINTERVAL_ADDRESS, oilerEmergencyInterval);
    EEPROM.put(OILER_LEVEL_ADDRESS, oilerLevel);
    EEPROM.put(OILER_PUMPDURATION_ADDRESS, oilerPumpDuration);
    EEPROM.put(OILER_PUMPIMPULSES_ADDRESS, oilerPumpImpulses);
    EEPROM.put(OILER_PUMPIMPULSEVOLUME_ADDRESS, oilerPumpImpulseVolume);
    EEPROM.put(OILER_REARSPROCKETTEETH_ADDRESS, oilerRearSprocketTeeth);
    EEPROM.put(OILER_RESERVOIR_ADDRESS, oilerReservoir);
    EEPROM.put(OILER_ROTATIONLENGTH_ADDRESS, oilerRotationLength);
    EEPROM.put(OILER_SPROCKETTEETH_ADDRESS, oilerSprocketTeeth);
    EEPROM.put(OILER_TICKPERROTATION_ADDRESS, oilerTickPerRotation);

    EEPROM.put(TOTALDISTANCE_ADDRESS, totalDistance);
    EEPROM.commit();
  }

  int getHeaterBalance() {
    return heaterBalance;
  }

  void setHeaterBalance(int balance) {
    heaterBalance = balance;
  }

	int getHeaterMaxPower() {
		return heaterMaxPower;
	}

  void setHeaterMaxPower(int heaterMaxPower) {
    this->heaterMaxPower = heaterMaxPower;
  }
  
	int getHeaterMaxTemp() {
		return heaterMaxTemp;
	}

  void setHeaterMaxTemp(int heaterMaxTemp) {
    this->heaterMaxTemp = heaterMaxTemp;
  }

	int getHeaterStartPower() {
		return heaterStartPower;
	}

  void setHeaterStartPower(int heaterStartPower) {
    this->heaterStartPower = heaterStartPower;
  }

	int getHeaterStartTemp() {
		return heaterStartTemp;
	}

  void setHeaterStartTemp(int heaterStartTemp) {
    this->heaterStartTemp = heaterStartTemp;
  }

	int getOilerDistance() {
		return oilerDistance;
	}

  void setOilerDistance(int oilerDistance) {
    this->oilerDistance = oilerDistance;
  }

	int getOilerEmergencyInterval() {
		return oilerEmergencyInterval;
	}

  void setOilerEmergencyInterval(int oilerEmergencyInterval) {
    this->oilerEmergencyInterval = oilerEmergencyInterval;
  }

 float getOilerLevel() {
    return oilerLevel;
  }

  void setOilerLevel(float oilerLevel) {
    this->oilerLevel = oilerLevel;
  }

  void resetOilerLevel() {
    information->oilLevelPercent = 100;
    setOilerLevel(information->oilLevelPercent);
    persist();
  }

 int getOilerPumpDuration() {
    return oilerPumpDuration;
  }

  void setOilerPumpDuration(int oilerPumpDuration) {
    this->oilerPumpDuration = oilerPumpDuration;
  }

 int getOilerPumpImpulses() {
    return oilerPumpImpulses;
  }

  void setOilerPumpImpulses(int oilerPumpImpulses) {
    this->oilerPumpImpulses = oilerPumpImpulses;
  }

 int getOilerPumpImpulseVolume() {
    return oilerPumpImpulseVolume;
  }

  void setOilerPumpImpulseVolume(int oilerPumpImpulseVolume) {
    this->oilerPumpImpulseVolume = oilerPumpImpulseVolume;
  }

 int getOilerRearSprocketTeeth() {
    return oilerRearSprocketTeeth;
  }

  void setOilerRearSprocketTeeth(int oilerRearSprocketTeeth) {
    this->oilerRearSprocketTeeth = oilerRearSprocketTeeth;
  }

 int getOilerReservoir() {
    return oilerReservoir;
  }

  void setOilerReservoir(int oilerReservoir) {
    this->oilerReservoir = oilerReservoir;
  }

 int getOilerRotationLength() {
    return oilerRotationLength;
  }

  void setOilerRotationLength(int oilerRotationLength) {
    this->oilerRotationLength = oilerRotationLength;
  }

 int getOilerSprocketTeeth() {
    return oilerSprocketTeeth;
  }

  void setOilerSprocketTeeth(int oilerSprocketTeeth) {
    this->oilerSprocketTeeth = oilerSprocketTeeth;
  }

  int getOilerTickPerRotation() {
    return oilerTickPerRotation;
  }

  void setOilerTickPerRotation(int oilerTickPerRotation) {
    this->oilerTickPerRotation = oilerTickPerRotation;
  }

  unsigned int getTotalDistance() {
    return totalDistance;
  }

  void setTotalDistance(unsigned int totalDistance) {
    this->totalDistance = totalDistance;
  }

private:
  Information *information;
  int heaterBalance;
  int heaterMaxPower;
  int heaterMaxTemp;
  int heaterStartPower;
  int heaterStartTemp;
	int oilerDistance;
	int oilerEmergencyInterval;
  float oilerLevel;
  int oilerPumpDuration;
  int oilerPumpImpulses;
  int oilerPumpImpulseVolume;
  int oilerRearSprocketTeeth;
  int oilerReservoir;
  int oilerRotationLength;
  int oilerSprocketTeeth;
  int oilerTickPerRotation;
  unsigned int totalDistance;
};

#endif /* SETTINGS_H_ */
