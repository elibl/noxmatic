class Pwm {
public:
  Pwm(int pinPwm) {
    this->pinPwm = pinPwm;
    this->pinActive = false;
    this->power = 0;
    this->nextMillis = 0;
  }

  ~Pwm() {
  }

  void setPower(int power) {
    this->power = power;
  }

  void init() {
    pinMode(pinPwm, OUTPUT);
    deactivatePin();
  }
  
  void process() {
    runPwm();
  }

private:
  int pinPwm;
  bool pinActive;
  int power;
  unsigned long nextMillis;

  void runPwm() {
    unsigned long currentMillis = millis();
  
    if (nextMillis < currentMillis) {
      if (!pinActive && power > 0) {
        activatePin();
        nextMillis = currentMillis + calculateOnMillis();
      } else {
        deactivatePin();
        nextMillis = currentMillis + calculateOffMillis();
      }
    }
  }
  
  void activatePin() {
    pinActive = true;
    digitalWrite(pinPwm, HIGH);
  }
  
  void deactivatePin() {
    pinActive = false;
    digitalWrite(pinPwm, LOW);
  }
  
  long calculateOffMillis() {
    return 1000 - power * 10;
  }
  
  long calculateOnMillis() {
    return power * 10;
  }
};
