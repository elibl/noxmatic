#ifndef INFORMATION_H_
#define INFORMATION_H_

class Information {
public:
  int connectWifiRetry;
  int distancePercent;
  int heaterPower;
  int speed;
  int temperature;
  bool pumpActive;
  bool pumpPending;
  bool rain;
  bool speedSignalLost;
  String ip;
};
#endif /* INFORMATION_H_ */
