#ifndef INFORMATION_H_
#define INFORMATION_H_

class Information {
public:
  bool pumpActive;
  bool pumpPending;
  bool rain;
  bool speedSignalLost;
  int connectWifiRetry;
  int distancePercent;
  int heaterPower;
  float oilLevelPercent;
  int speed;
  int temperature;
  unsigned int distance = 0;
  unsigned int ticks = 0;
  String ip;
};
#endif /* INFORMATION_H_ */
