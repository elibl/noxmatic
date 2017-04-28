#include "Settings.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* config_ssid = "noxmatic";
const char* config_password = "noxmatic";

ESP8266WebServer server(80);

class CommunicationESP {

public:
  CommunicationESP(Settings *settings) {
      this->settings = settings;
  }
  
  virtual ~CommunicationESP() {    
  }

  bool connectWifi() {
    WiFi.disconnect(); 
    WiFi.mode(WIFI_STA);
    WiFi.begin(config_ssid, config_password);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED) {
      retry++;
      if (retry > 50) {
        return false;
      }
      delay(500);
    }

    server.begin();
    server.on("/", std::bind(&CommunicationESP::sendHtml, this));

    Serial.println(WiFi.localIP());

    return true;
  }
  
  void process() {
    server.handleClient();
  }

private:
  Settings *settings;

  void sendHtml() {
    String action = server.arg("action");
    String add = "";
    if (action == "save") {
      settings->setHeaterStartTemp(getValue("heaterStartTemp", 10));
      settings->setHeaterMaxTemp(getValue("heaterMaxTemp", 10));
      settings->setHeaterStartPower(getValue("heaterStartPower", 1));
      settings->setHeaterMaxPower(getValue("heaterMaxPower", 1));
      settings->setOilerTickPerRotation(getValue("oilerTickPerRotation", 1));
      settings->setOilerRotationLength(getValue("oilerRotationLength", 1));
      settings->setOilerDistance(getValue("oilerDistance", 1));
      settings->setOilerEmergencyInterval(getValue("oilerEmergencyInterval", 1));
      settings->persist();
    }

    String temp = "<html><body><form action=\"\" method=\"post\"><input type=\"hidden\" name=\"action\" id=\"action\" value=\"save\"><table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">";
    temp += "<thead><tr><th>Setting</th><th>Aktueller Wert</th><th>Neuer Wert</th></tr></thead><tbody>";
    temp += "<tr><td colspan=\"3\"></td></tr>";
    temp += "<tr><td colspan=\"3\">Heater</td></tr>";
    temp += buildRow("heaterStartTemp", settings->getHeaterStartTemp(), "0.5", 10, 0, 25);
    temp += buildRow("heaterStartPower", settings->getHeaterStartPower(), "5", 1, 0, 100);
    temp += buildRow("heaterMaxTemp", settings->getHeaterMaxTemp(), "0.5", 10, 0, 25);
    temp += buildRow("heaterMaxPower", settings->getHeaterMaxPower(), "5", 1, 0, 100);
  
    temp += "<tr><td colspan=\"3\"></td></tr>";
    temp += "<tr><td colspan=\"3\">Oiler</td></tr>";
    temp += buildRow("oilerTickPerRotation", settings->getOilerTickPerRotation(), "1", 1, 1, 255);
    temp += buildRow("oilerRotationLength", settings->getOilerRotationLength(), "10", 1, 10, 2550);
    temp += buildRow("oilerDistance", settings->getOilerDistance(), "100", 1, 100, 25500);
    temp += buildRow("oilerEmergencyInterval", settings->getOilerEmergencyInterval(), "10", 1, 10, 2550);
    temp += "</tbody></table><button type=\"submit\">Speichern</button></form></body></html>";
  
    temp += add;
    server.send(200, "text/html", temp);
  }
  
  String buildRow(String property, int value, String step, int factor, int min, int max) {
    float floatVal = (float) value / factor;
    String row = "<tr><td>";
    row += property;
    row += "</td><td><input readonly type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"en-150\" value=\"";
    row += floatVal;
    row += "\" /></td><td><input type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"en-150\" value=\"";
    row += floatVal;
    row += "\" name=\"";
    row += property;
    row += "\" min=\"";
    row += min;
    row += "\" max=\"";
    row += max;
    row += "\" /></td></tr>";
    return row;
  }
  
  int getValue(String property, int factor) {
    String value = server.arg(property);
    return value.toFloat() * factor;
  }
};
