#include <ArduinoOTA.h>
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
    if (information->connectWifiRetry < 20) {

      unsigned long currentMillis = millis();

      if (currentMillis > nextRefreshMillis) {
        nextRefreshMillis = currentMillis + REFRESH_INTERVAL_MILLIS;
        information->connectWifiRetry++;

        if (WiFi.status() == WL_CONNECTED) {
          httpUpdater.setup(&server);
          server.on("/", std::bind(&CommunicationESP::sendHtml, this));
          server.on("/getData", std::bind(&CommunicationESP::handleData, this));
          server.begin();
          ArduinoOTA.begin();

          information->ip = WiFi.localIP().toString();
          information->connectWifiRetry = 20;
        }
      }
    }
    else if (WiFi.status() == WL_CONNECTED) {
      ArduinoOTA.handle();
      server.handleClient();
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
  unsigned long nextRefreshMillis = 0;

  void handleData() {
    String data = (String)information->speed + " km/h|";
    data += (String)(settings->getTotalDistance() * pow(10,-3)) + " km|";
    data += (String)information->ticks + " Radticks|";
    data += (String)(information->distance * pow(10,-6)) + " km|";
    data += (String)information->oilLevelPercent + "% Öl";
    server.send(200, "text/plain", data);
  }

  void sendHtml() {
    String action = server.arg("action");
    if (action == "save") {
      settings->setHeaterStartTemp(getValue("heaterStartTemp", 10));
      settings->setHeaterMaxTemp(getValue("heaterMaxTemp", 10));
      settings->setHeaterStartPower(getValue("heaterStartPower", 1));
      settings->setHeaterMaxPower(getValue("heaterMaxPower", 1));
      settings->setHeaterBalance(getValue("heaterBalance", 1));
      settings->setOilerDistance(getValue("oilerDistance", 1));
      settings->setOilerEmergencyInterval(getValue("oilerEmergencyInterval", 1));
      settings->setOilerPumpDuration(getValue("oilerPumpDuration", 1));
      settings->setOilerPumpImpulses(getValue("oilerPumpImpulses", 1));
      settings->setOilerPumpImpulseVolume(getValue("oilerPumpImpulseVolume", 1));
      settings->setOilerRearSprocketTeeth(getValue("oilerRearSprocketTeeth", 1));
      settings->setOilerReservoir(getValue("oilerReservoir", 1));
      settings->setOilerRotationLength(getValue("oilerRotationLength", 1));
      settings->setOilerSprocketTeeth(getValue("oilerSprocketTeeth", 1));
      settings->setOilerTickPerRotation(getValue("oilerTickPerRotation", 1));
      settings->persist();
    } else if (action == "reset") {
      information->ticks = 0;
      information->distance = 0;
      settings->resetOilerLevel();
    } else if (action == "pump") {
      pump->pump();
    }

    String temp = "<html>";
    temp += R"=====(
              <head>
              <meta http-equiv="content-type" content="text/html; charset=utf-8">
              <meta name="viewport" content="width=device-width, initial-scale=1.0">
              <script>
              setInterval(getData, 500);

              function getData() {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                  if (this.readyState == 4 && this.status == 200) {
                    var tmpArray = this.responseText.split("|");
                    document.getElementById("speed").innerHTML = tmpArray[0];
                    document.getElementById("totalDistance").innerHTML = tmpArray[1];
                    document.getElementById("ticks").innerHTML = tmpArray[2];
                    document.getElementById("distance").innerHTML = tmpArray[3];
                    document.getElementById("oilLevel").innerHTML = tmpArray[4];
                  }
                };
                xhttp.open("GET", "getData", true);
                xhttp.send();
                }
              </script>
              </head>
            )=====";
    temp += "<body>";
    temp += "<table border=\"1\"  width=\"100%\" cellspacing=\"0\" cellpadding=\"5\" style=\"white-space:nowrap\">";
    temp += "<thead><tr><th colspan=\"3\" style=\"font-size:300%; padding:20\" id=\"speed\">" + (String)information->speed + " km/h</th></tr>";
    temp += "<tr><th colspan=\"3\" padding:20\" id=\"totalDistance\">" + (String)(settings->getTotalDistance() * pow(10,-3)) + " km</th></tr>";
    temp += "<tr><th colspan=\"2\" padding:20\" id=\"ticks\">" + (String)information->ticks + " Radticks</th>";
    temp += "<form action=\"\" method=\"post\"><input type=\"hidden\" name=\"action\" id=\"action\" value=\"reset\">";
    temp += "<td align=\"center\" rowspan=\"3\" style=\"padding:10\"><button type=\"submit\">Zurücksetzen</button></td></form>";
    temp += "<tr><th colspan=\"2\" padding:20\" id=\"distance\">" + (String)information->distance + " km</th></tr>";
    temp += "<tr><th colspan=\"2\" padding:20\" id=\"oilLevel\">" + (String)information->oilLevelPercent + "% Öl</th></tr></thead>";
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
    temp += buildRow("oilerTickPerRotation", "Radticks", settings->getOilerTickPerRotation(), "1", 1, 1, pow(2,31) - 1);
    temp += buildRow("oilerSprocketTeeth", "oilerRearSprocketTeeth", "Übersetzung", settings->getOilerSprocketTeeth(), settings->getOilerRearSprocketTeeth(), "1", 1, 1, pow(2,31) - 1);
    temp += buildRow("oilerRotationLength", "Radumfang [mm]", settings->getOilerRotationLength(), "10", 1, 10, pow(2,31) - 1);
    temp += buildRow("oilerDistance", "Distanz [m]", settings->getOilerDistance(), "100", 1, 100, pow(2,31) - 1);
    temp += buildRow("oilerEmergencyInterval", "Notintervall [s]", settings->getOilerEmergencyInterval(), "10", 1, 10, pow(2,31) - 1);
    temp += buildRow("oilerPumpDuration", "Pumpdauer [ms]", settings->getOilerPumpDuration(), "10", 1, 10, pow(2,31) - 1);
    temp += buildRow("oilerPumpImpulses", "Pumpimpulse", settings->getOilerPumpImpulses(), "1", 1, 1, pow(2,31) - 1);
    temp += buildRow("oilerPumpImpulseVolume", "Pumpimpulsmenge [nl]", settings->getOilerPumpImpulseVolume(), "100", 1, 100, pow(2,31) - 1);
    temp += buildRow("oilerReservoir", "Tankvolumen [ml]", settings->getOilerReservoir(), "1", 1, 1, pow(2,31) - 1);
    temp += "</form><tr><td colspan=\"2\"></td><td></td></tr>";
    temp += "<form action=\"\" method=\"post\"><input type=\"hidden\" name=\"action\" id=\"action\" value=\"pump\">";
    temp += "<tr><td align=\"center\" colspan=\"2\" style=\"padding:10\"><button type=\"submit\">Pumpen</button></td></form>";
    temp += "<td align=\"center\" style=\"padding:10\"><button form=\"save\" type=\"submit\">Speichern</button></td></tr>";
    temp += "</tbody></table></body></html>";
    server.send(200, "text/html", temp);
  }

  String buildRow(String property, String text, int value, String step, int factor, int min, int max) {
    int val = value / factor;
    String row = "<tr><td>";
    row += text;
    row += "</td><td><input style=\"text-align: right; width: 100%\" readonly type=\"number\" lang=\"de-DE\" value=\"";
    row += val;
    row += "\" /></td><td><input style=\"text-align: right; width: 100%\" type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"de-DE\" value=\"";
    row += val;
    row += "\" name=\"";
    row += property;
    row += "\" min=\"";
    row += min;
    row += "\" max=\"";
    row += max;
    row += "\" /></td></tr>";
    return row;
  }

  String buildRow(String property, String property2, String text, int value, int value2, String step, int factor, int min, int max) {
    int val = value / factor;
    int val2 = value2 / factor;
    String row = "<tr><td>";
    row += text;
    row += "</td><td style=\"text-align: right\"><input style=\"text-align: right; width: 40%\" readonly type=\"number\" lang=\"de-DE\" value=\"";
    row += val;
    row += "\" /> / ";
    row += "<input style=\"text-align: right; width: 40%\" readonly type=\"number\" lang=\"de-DE\" value=\"";
    row += val2;
    row += "\" /></td><td style=\"text-align: right\"><input style=\"text-align: right; width: 40%\" type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"de-DE\" value=\"";
    row += val;
    row += "\" name=\"";
    row += property;
    row += "\" min=\"";
    row += min;
    row += "\" max=\"";
    row += max;
    row += "\" /> / ";
    row += "<input style=\"text-align: right; width: 40%\" type=\"number\" step=\"";
    row += step;
    row += "\" lang=\"de-DE\" value=\"";
    row += val2;
    row += "\" name=\"";
    row += property2;
    row += "\" min=\"";
    row += min;
    row += "\" max=\"";
    row += max;
    row += "\" /></td></tr>";
    return row;
  }
  
  int getValue(String property, int factor) {
    String value = server.arg(property);
    return value.toInt() * factor;
  }
};
