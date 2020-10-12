#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include "Information.h"
#include "Settings.h"

#define REFRESH_INTERVAL_MILLIS 500;

const char* config_ssid = "noxmatic";
const char* config_password = "noxmatic";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

class CommunicationESP {

public:
  CommunicationESP(Information *information, Pump *pump, Settings *settings) {
      this->information = information;
      this->pump = pump;
      this->settings = settings;
  }
  
  virtual ~CommunicationESP() {    
  }

  void init() {
    WiFi.disconnect(); 
    WiFi.mode(WIFI_STA);
    WiFi.begin(config_ssid, config_password);
    information->connectWifiRetry = 0;
  }
  
  void process() {
    if (information->connectWifiRetry < 20 || WiFi.status() == WL_CONNECTED) {
      static unsigned long nextRefreshMillis = 0;

      unsigned long currentMillis = millis();
      if (currentMillis > nextRefreshMillis) {
        nextRefreshMillis = currentMillis + REFRESH_INTERVAL_MILLIS;
        if (information->ip.length() > 0 && WiFi.status() == WL_CONNECTED) {
          server.handleClient();
        }
        else if (WiFi.status() != WL_CONNECTED) {
          information->connectWifiRetry++;
        }
        else {
          httpUpdater.setup(&server);
          server.begin();
          server.on("/", std::bind(&CommunicationESP::sendHtml, this));
          server.on("/speed", std::bind(&CommunicationESP::handleSpeed, this));

          information->ip = WiFi.localIP().toString();
          information->connectWifiRetry = 20;
        }
      }
    }
    else if (WiFi.getMode() != WIFI_OFF){
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.forceSleepBegin();
    }
  }

private:
  Information *information;
  Pump *pump;
  Settings *settings;

  void handleSpeed() {
    String speed = (String)information->speed + " km/h";
    server.send(200, "text/plain", speed);
  }

  void sendHtml() {
    String action = server.arg("action");
    String add = "";
    if (action == "save") {
      settings->setHeaterStartTemp(getValue("heaterStartTemp", 10));
      settings->setHeaterMaxTemp(getValue("heaterMaxTemp", 10));
      settings->setHeaterStartPower(getValue("heaterStartPower", 1));
      settings->setHeaterMaxPower(getValue("heaterMaxPower", 1));
      settings->setHeaterBalance(getValue("heaterBalance", 1));
      settings->setOilerTickPerRotation(getValue("oilerTickPerRotation", 1));
      settings->setOilerRotationLength(getValue("oilerRotationLength", 1));
      settings->setOilerDistance(getValue("oilerDistance", 1));
      settings->setOilerEmergencyInterval(getValue("oilerEmergencyInterval", 1));
      settings->setOilerPumpDuration(getValue("oilerPumpDuration", 1));
      settings->persist();
    } else if (action == "pump") {
      pump->pump();
    }

    String temp = "<html>";
    temp += R"=====(
              <head>
              <meta http-equiv="content-type" content="text/html; charset=utf-8">
              <meta name="viewport" content="width=device-width, initial-scale=1.0">
              <script>
              setInterval(function() {
                getData();
              }, 500);

              function getData() {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                  if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("speed").innerHTML =
                    this.responseText;
                  }
                };
                xhttp.open("GET", "speed", true);
                xhttp.send();
                }
              </script>
              </head>
            )=====";
    temp += "<body>";
    temp += "<table border=\"1\"  width=\"100%\" cellspacing=\"0\" cellpadding=\"5\" style=\"white-space:nowrap\">";
    temp += "<thead><tr><th colspan=\"3\" style=\"font-size:300%; padding:20\" id=\"speed\">0 km/h</th></tr></thead>";
    temp += "<tr><td colspan=\"3\"></td></tr>";
    temp += "<tbody><form action=\"\" method=\"post\" id=\"save\"><input type=\"hidden\" name=\"action\" id=\"action\" value=\"save\">";
    temp += "<tr><th>Heizung</th><th>Aktuell</th><th>Neu</th></tr>";
    temp += buildRow("heaterStartTemp", "Ein Temperatur", settings->getHeaterStartTemp(), "0.5", 10, 0, 25);
    temp += buildRow("heaterStartPower", "Ein Leistung", settings->getHeaterStartPower(), "5", 1, 0, 100);
    temp += buildRow("heaterMaxTemp", "Max Temperatur", settings->getHeaterMaxTemp(), "0.5", 10, 0, 25);
    temp += buildRow("heaterMaxPower", "Max Leistung", settings->getHeaterMaxPower(), "5", 1, 0, 100);
    temp += buildRow("heaterBalance", "Gas Anteil", settings->getHeaterBalance(), "5", 1, 0, 100);
    temp += "<tr><td colspan=\"2\"></td><td></td></tr>";
    temp += "<tr><th>Öler</th><th>Aktuell</th><th>Neu</th></tr>";
    temp += buildRow("oilerTickPerRotation", "Radticks [Anzahl]", settings->getOilerTickPerRotation(), "1", 1, 1, 255);
    temp += buildRow("oilerRotationLength", "Radumfang [mm]", settings->getOilerRotationLength(), "10", 1, 10, 2550);
    temp += buildRow("oilerDistance", "Distanz [m]", settings->getOilerDistance(), "100", 1, 100, 25500);
    temp += buildRow("oilerEmergencyInterval", "Notintervall [s]", settings->getOilerEmergencyInterval(), "10", 1, 10, 2550);
    temp += buildRow("oilerPumpDuration", "Pumpdauer [ms]", settings->getOilerPumpDuration(), "10", 1, 10, 2550);
    temp += "</form><tr><td colspan=\"2\"></td><td></td></tr>";
    temp += "<form action=\"\" method=\"post\"><input type=\"hidden\" name=\"action\" id=\"action\" value=\"pump\">";
    temp += "<tr><td align=\"center\" colspan=\"2\" style=\"padding:10\"><button type=\"submit\">Pumpen</button></td></form>";
    temp += "<td align=\"center\" style=\"padding:10\"><button form=\"save\" type=\"submit\">Speichern</button></td></tr>";
    temp += "</tbody></table></body></html>";
  
    temp += add;
    server.send(200, "text/html", temp);
  }
  
  String buildRow(String property, String text, int value, String step, int factor, int min, int max) {
    float floatVal = (float) value / factor;
    String row = "<tr><td>";
    row += text;
    row += "</td><td><input style=\"direction:rtl;width:100%\" readonly type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"en-150\" value=\"";
    row += floatVal;
    row += "\" /></td><td><input style=\"direction:rtl;width:100%\" type=\"number\" step=\"";
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
