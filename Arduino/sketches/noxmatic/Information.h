#ifndef INFORMATION_H_
#define INFORMATION_H_

class Information {
public:
  int connectWifiRetry;
  int distancePercent;
  int heaterPower;
  int speed;
  int temperature;
  bool pumpPending;
  bool speedSignalLost;
  String ip;
};
#endif /* INFORMATION_H_ */
