# SBUS to PPM Converter (2 RC-Servo Outputs)

**Hardware:** ATmega328P with Hardware Inverter for RX (SBUS-In)
**Author:** frittna
**Date:** 14.10.2026

---

##### RUNTER SCROLLEN FÜR DEUTSCH -- SCROLL DOWN FOR GERMAN #####

## Operating Instructions

### Regular Boot
Upon power-on, both LEDs flash simultaneously 2x. Following a short pause, the LEDs will flash extremely fast for a short time as soon as an SBUS signal is detected.

Immediately after, the assigned channels are output as PPM signals to the servos, which should now react to the programmed channels on the radio.

---

## Configuration & Setups

### Programming Channels (Standard Setup)
1. **Enter Setup:** Press and hold the button while applying 5V power. Release the button after 1–2 seconds.
   * *Note: Do not hold it longer than 6–7 seconds, as this triggers the extended signal inversion setup instead.*
2. **Confirmation:** Both LEDs flash 3x quickly to confirm entry into the channel setup. After a brief pause, the routine begins.
3. **Status Output:** Before configuring, the LEDs display the currently saved status:
   * **Servo 1:** LED 1 (Green) blinks the active channel number (e.g., 7 flashes = Channel 7).
   * *Pause: 1.5 seconds.*
   * **Servo 2:** LED 2 (Blue) blinks the active channel number in the same manner.

#### Configuring Servo Channel 1
* LED 1 (Green) lights up permanently, indicating **Servo 1** is selected.
* LED 2 (Blue) starts counting through channels 1 to 16.
* **Pattern per channel:** 150 ms ON, then 500 ms OFF, followed by a 1000 ms timeout.
* **Action:** Count the flashes of LED 2 (1 flash = Channel 1, 2 flashes = Channel 2, etc.). Once the desired channel is reached, press the button briefly (valid either while the LED is ON or during the subsequent pause).
* **Success:** LED 1 (Green) flashes 3x at medium speed (300 ms ON / 300 ms OFF). The channel is saved.
* *Pause: 1.5 seconds.*

#### Configuring Servo Channel 2
* The procedure is identical to Servo 1, but reversed: LED 2 (Blue) lights up permanently, indicating **Servo 2** is selected.
* LED 1 (Green) starts counting through channels 1 to 16 (150 ms ON, 500 ms OFF, 1000 ms timeout).
* **Action:** Count the flashes of LED 1. Once the desired channel is reached, press the button briefly again.
* **Success:** LED 2 (Blue) flashes 3x at medium speed (333 ms ON / 333 ms OFF).

---

### Extended Signal Inversion Setup
*Use this setup to toggle the SBUS-IN signal logic, which is normally hardware-inverted.*

1. **Enter Extended Setup:** Press and hold the button for more than 6 seconds during power-on.
2. **Confirmation:** Both LEDs flash 5x quickly to confirm entry.
3. **Status Output:** The current logic state is indicated by the flash rhythm:
   * **Inverted (Standard):** LEDs stay ON long (**850 ms ON / 150 ms OFF**).
     *Inverted TTL logic — Idle state = HIGH (Typical case for hardware inverter).*
   * **Non-inverted:** LEDs flash only briefly (**150 ms ON / 850 ms OFF**).
     *Non-inverted TTL logic — Idle state = LOW.*
4. **Toggle Mode:** Press the button briefly (< 800 ms) to switch between modes.
5. **Save:** Press and hold the button (> 800 ms) to permanently save the selected logic.
6. **Success:** Both LEDs flash 3x for confirmation, followed by a pause and the transition to regular boot mode.

---

## Error Handling

### Invalid Entries or No Input during Setup
If the button is not pressed at all during the setup routine and all 16 channels pass completely, the process aborts. **Nothing is saved**, and the entire setup routine automatically restarts from Phase 1 (Status Output) to prevent faulty configurations.



##### DEUTSCH/GERMAN #####


# SBUS zu PPM Konverter (2 RC-Servoausgänge)

**Hardware:** Atmega 328P mit Hardware-Inverter für RX (SBUS-In)
**Autor:** frittna
**Datum:** 14.10.2026

---

## Bedienungsanleitung

### Regulärer Start
Die LEDs blinken bei Start 2x kurz gleichzeitig. Dann kommt eine Pause und sobald SBUS erkannt wird blitzen die LEDs für kurze Zeit ganz schnell auf.

Direkt danach geht der jeweils zugeordnete Kanal als PPM Signal an die Servos und sollten auf die programmierten Kanäle an der Fernsteuerung reagieren.

---

## Konfiguration & Setups

### Kanäle einprogrammieren (Standard Setup)
1. **Setup starten:** Den Button gedrückt halten und 5V Power anlegen -> den Knopf aber nach 1–2 sek wieder auslassen.
   * *Hinweis: Nicht länger als 6–7 sek halten! Das wäre das Signal-invertier-Setup.*
2. **Bestätigung:** Beide LEDs blinken 3x schnell als Bestätigung für das Kanal-Setup. Nach einer kurzen Pause startet das Setup.
3. **Status-Ausgabe:** Vorher geben die LEDs aber noch den aktuell gespeicherten Status aus:
   * **Servo 1:** LED 1 (Grün) blinkt für Servo 1 den Kanal (z. B. 7x Blinken = Kanal 7).
   * *Pause: 1,5 Sekunden.*
   * **Servo 2:** LED 2 (Blau) blinkt für Servo 2 den Kanal genau so an.

#### Servokanal 1 einstellen
* LED 1 (Grün) leuchtet jetzt dauerhaft, bedeutet: **Servo 1** is jetzt dran.
* LED 2 (Blau) fängt nun an, die Kanäle von 1 bis 16 durchzuzählen.
* **Muster pro Kanal:** 150 ms AN, then 500 ms AUS, gefolgt von 1000 ms Bedenkzeit.
* **Aktion:** Das Blinken von LED 2 mitzählen (1x Blinken = Kanal 1, 2x Blinken = Kanal 2 usw.). Sobald Wunschkanal erreicht ist, kurz den Button drücken (während die LED an ist oder in der Pause danach ist egal).
* **Erfolg:** Bei Erfolg blinkt LED 1 (Grün) 3x mittelschnell (300 ms an / 300 ms aus). Kanal ist jetzt fest gespeichert.
* *Pause: 1,5 Sekunden.*

#### Servokanal 2 einstellen
* Das ganze ist nun wie bei Servo 1 aber diesmal seitenverkehrt: LED 2 (Blau) leuchtet jetzt dauerhaft, bedeutet: **Servo 2** ist an der Reihe.
* LED 1 (Grün) fängt nun an, die Kanäle von 1 bis 16 durchzuzählen (150 ms AN, 500 ms AUS, 1000 ms Bedenkzeit).
* **Aktion:** Das Blinken von LED 1 mitzählen. Sobald Wunschkanal erreicht ist wieder kurz den Button drücken.
* **Erfolg:** Bei Erfolg blinkt diesmal LED 2 (Blau) 3x mittelschnell (333 ms an / 333 ms aus).

---

### Erweitertes Signal-invertier-Setup
*Verwende dieses Setup, um die Signal-Logik des SBUS-IN umzuschalten, welche normalerweise invertiert ist.*

1. **Erweitertes Setup starten:** Den Button beim Einschalten länger als 6 Sekunden gedrückt halten.
2. **Bestätigung:** Beide LEDs blinken 5x schnell als Bestätigung für das erweiterte Setup.
3. **Status-Ausgabe:** Der aktuelle Status wird über den Blinkrhythmus ausgegeben:
   * **Invertiert:** LEDs leuchten lang (**850 ms AN / 150 ms AUS**).
     *Invertierte TTL Logik — Ruhezustand = HIGH — meist der Standardfall.*
   * **Nicht-invertiert:** LEDs leuchten nur kurz auf (**150 ms AN / 850 ms AUS**).
     *Nicht-invertierte TTL Logik — Ruhezustand = LOW.*
4. **Modus umschalten:** Taste kurz (< 800 ms) drücken um die Art umzuschalten.
5. **Speichern:** Langer Tastendruck (> 800 ms) speichert die gewählte Logik dauerhaft ab. Fertig.
6. **Erfolg:** Bei Erfolg blinken beide LEDs 3x zur Bestätigung, gefolgt von einer Pause und dem Normalmodus.

---

## Fehlerbehandlung

### Bei Falscheingaben oder keinen Eingaben im Setup
Wenn im Setup der Taster überhaupt nicht gedrückt wird und alle 16 Kanäle komplett durchlaufen, bricht der Durchlauf ab. **Es wird nichts gespeichert** und der gesamte Setup-Prozess startet einfach automatisch wieder bei Phase 1 (Status-Ausgabe) von vorne, um Fehlkonfigurationen zu vermeiden.
