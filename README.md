# Kettenöler und Heizgriffsteuerung für Motorrad

## Eckdaten
- Vollautomatische wegstreckenabhängige Kettenölung
- Vollautomatische außentemperaturabhängige Griffheizung (getrennt regelnde Griffe)
- Display mit Anzeige von Temperatur, Geschwindigkeit, Streckenintervall und Heizleistung
- LED zur Statusanzeige
- Taster zur Steuerung von Basisfunktionen
- Bequeme Konfiguration über WiFi im Browser
- OTA Updatefähig
- Teilaufbau möglich (nur Kettenöler, nur Griffheizung, mit und ohne Display, LED oder Taster)

## Funktionalität

### LED Blink Codes
- 4x im 1 Sekunden Takt beim Starten
- An während Pumpe aktiv
- Dauerhaft im 1 Sekunden Takt -> Regenmodus aktiv
- Dauerhaft im "Öltank Restvolumen in Prozent" Sekunden Takt wenn Öltank unter 20%
- Dauerhaft im 250 Millisekunden Takt -> kein Tachosignal seit 10 Minuten

### Taster
- 1x drücken für Rengenmodus (10-fache Ölmenge)
- 4x schnell (500ms) drücken für Öltank zurücksetzen
- 1x lang (5s) drücken für Pumpenaktivierung

## Komponenten

#### Basis (wird immer gebraucht)
- 1x NodeMCU als Controller (incl. Wifi)
- 1x Zenerdiode 15V 5W
- 1x Drosselspule 47µF (min 200mA)
- 1x Anschlussklemmen

Hiermit kann man den Controller schonmal am Motorrad betreiben.

#### Kettenölung
- 1x Bat42 (Tachosignal)
- 1x Kondensator 1nF (Tachosignal)
- 1x R100
- 1x R10k
- 1x IRF3708

Hiermit kann die Kettenölung mit Geschwindigkeitsmessung ergänzt werden.

#### Heizgriffsteuerung
- 1x R1k
- 2x R100
- 2x R10k
- 2x IRF3708
- 1x DS18B80

Hiermit kann die Griffheizung samt Temperaturmessung ergänzt werden.

#### Display
- 1x DOGS102-6
- 1x LED Hintegrundbeleuchtung für DOGS102
- 3x Kondensator 1µF
- 1x R1k
- 1x R10k
- 2x Buchse für min 6 Adern Kabel RJ45 bietet sich an, dann kann man geschirmte Netzwerkkabel verwenden

Hiermit kann das Display ergänzt werden.
