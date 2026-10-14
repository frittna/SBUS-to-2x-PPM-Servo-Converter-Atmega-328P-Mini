/*
//### SBUS zu PPM Konverter mit 2 RC-Servoausgängen. Hardware: Atmega 328P mit Hardware-Inverter für RX (SBUS-In) frittna @ 14.10.2026

//BEDIENUNGSANLEITUNG

## Regulärer Start
   Die LEDs blinken bei Start 2x kurz gleichzeitig. Dann kommt eine Pause und soblad SBUS erkannt wird blitzen die LEDs kurz ganz schnell auf.
  Direkt danach geht der jeweils zugeordnete Kanal als PPM Signal an die Servos und sollten auf die programmierten Kanäle an der Fernsteuerung reagieren.

## Kanäle einprogrammieren - Setup:
1. Den Button gedrückt halten und 5V Power anlegen -> den Knopf aber nach 1-2 sek wieder auslassen (nicht länger als 6-7sek! das wäre das Signal-invertier-Setup*)
   Beide LEDs blinken 3x schnell als Bestätigung für das Kanal-Setup.
   Nach einer kurzen Pause startet das Setup um gleich abzufragen welche 2 Servos einzeln ausgegeben werden sollen.
2. Vorher geben die LEDs aber noch den aktuell gespeicherten Stauts aus. Servo1 (LED Grün) blinkt für Servo 1 den Kanal z. B. 7x Blinken = Kanal 7.
   Es folgt eine Pause von 1,5 Sekunden. Dann wird aktueller Status von Servo2 (LED Blau) genauso angezeigt.

## jetzt kommt: Servokanal 1 einstellen
• LED 1 (Grün) leuchtet jetzt dauerhaft, bedeutet: Servo 1 is jetzt dran.
• LED 2 (Blau) fängt nun an, die Kanäle von 1 bis 16 durchzuzählen. Das Muster pro Kanal ist: 150 ms AN, then 500 ms AUS, gefolgt von 1000 ms Bedenkzeit.
• Das Blinken von LED 2 mitzählen (1x Blinken = Kanal 1, 2x Blinken = Kanal 2 usw)
• Sobald Wunschkanal erreicht ist, kurz den Button drücken (während die LED an ist oder in der Pause danach ist egal).
• Bei Erfolg blinkt LED 1 (Grün)3x mittelschnell (300 ms an / 300 ms aus). Kanal ist jetzt fest gespeichert.
• Es folgt eine Pause von 1,5 Sekunden.

## jetzt: Servokanal 2 einstellen
• Das ganze ist nun wie bei Servo 1 aber diesmal seitenverkehrt. LED 2 (Blau) leuchtet jetzt dauerhaft. Das bedeutet: Servo 2 ist an der Reihe.
• LED 1 (Grün) fängt nun an, die Kanäle von 1 bis 16 durchzuzählen (150 ms AN, 500 ms AUS, 1000 ms Bedenkzeit).
• Das Blinken von LED 1 mitzählen (1x Blinken = Kanal 1, 2x Blinken = Kanal 2 usw)
• Sobald Wunschkanal erreicht ist wieder kurz den Button drücken.
• Bei Erfolg blinkt diesmal LED 2 (Blau) 3x mittelschnell (333 ms an / 333 ms aus).


* ## Erweitertes Signal-invertier-Setup (Signal-Logik des SBUS-IN umschalten welche normalerweise invertiert ist)
1. Den Button beim Einschalten länger als 6 Sekunden gedrückt halten.
2. Beide LEDs blinken 5x schnell als Bestätigung für das erweiterte Setup.
3. Der aktuelle Status wird über den Blinkrhythmus ausgegeben:
   • Invertiert:       LEDs leuchten lang (850 ms AN / 150 ms AUS)                 invertierte TTL Logik - Ruhezust=HIGH - meist der Standardfall)
   • Nicht-invertiert: LEDs leuchten nur kurz auf (150 ms AN / 850 ms AUS)   nicht invertierte TTL logik - Ruhezust=LOW)
4. Taste kurze (< 800 ms) drücken um die Art umzuschalten.
5. Langer Tastendruck (> 800 ms) speichert die gewählte Logik dauerhaft ab. Fertig.
6. Bei Erfolg blinken beide LEDs 3x zur Bestätigung, gefolgt von einer Pause und dem Normalmodus.

 
>>> Bei Falscheingaben oder keinen Eingaben im Setup:
Wenn im Setup der Taster überhaupt nicht gedrückt wird und alle 16 Kanäle komplett durchlaufen, bricht der Durchlauf ab. 
Es wird nichts gespeichert und der gesamte Setup-Prozess startet einfach automatisch wieder bei Phase 1 von vorne.
*/

#include <EEPROM.h>

// Pin-Definitionen
const int PIN_SWITCH = A0;  // PC0 - Setup-Taster gegen GND
const int PIN_LED1 = 12;    // PB4 - LED 1 (Gruen)
const int PIN_LED2 = 13;    // PB5 - LED 2 (Blau)

// Timer 1 Hardware-PWM Ausgaenge
const int PIN_SERVO1 = 9;   // PB1 (OC1A) - Servo 1
const int PIN_SERVO2 = 10;  // PB2 (OC1B) - Servo 2

// EEPROM-Adressen
const int EEPROM_ADDR_CH1 = 0;
const int EEPROM_ADDR_CH2 = 1;
const int EEPROM_ADDR_INVERT = 2;

// Standardkanaele
uint8_t activeChannel1 = 7;
uint8_t activeChannel2 = 8;
uint8_t sbusInverted = 1;  // (Standard)

// Globale Variablen fuer Hardware-Serial
uint8_t sbusBuffer[25];
uint8_t bufferIndex = 0;
uint16_t sbusChannels[16];

// Zeitstempel fuer Datenstrom-Synchronisation
unsigned long lastByteTime = 0;

// SBUS Erkennungs-Animation
bool sbusDetected = false;
unsigned long sbusFirstDetectTime = 0;
bool sbusAnimationDone = false;

// Vorwärtsdeklarationen fuer den Kompiliervorgang
bool evaluateExtendedClick(unsigned long dauer);
void runExtendedSetup();
void runSetupMode();
void playNormalBootAnimation();
void blinkChannelInfo(int ledPin, uint8_t channelZeroIndexed);
int recordChannel(int activeLed, int blinkingLed);
void parseSBUS();

void setup() {
  delay(500);  // Boot-Verzoegerung

  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);

  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);

  // Parameter aus dem EEPROM laden
  uint8_t storedCh1 = EEPROM.read(EEPROM_ADDR_CH1);
  uint8_t storedCh2 = EEPROM.read(EEPROM_ADDR_CH2);
  uint8_t storedInvert = EEPROM.read(EEPROM_ADDR_INVERT);

  if (storedCh1 < 16) activeChannel1 = storedCh1;
  if (storedCh2 < 16) activeChannel2 = storedCh2;

  if (storedInvert == 255) {
    sbusInverted = 1;  // Standardmäßig Invertiert-Modus wegen deines Hardware-Inverters
  } else {
    sbusInverted = storedInvert;
  }

  // Weichenstellung fuer normales oder erweitertes Setup beim Booten
  if (digitalRead(PIN_SWITCH) == LOW) {
    unsigned long bootDruckZeit = 0;
    bool erweitertesSetupWunsch = false;

    while (digitalRead(PIN_SWITCH) == LOW) {
      delay(10);
      bootDruckZeit += 10;
      if (bootDruckZeit >= 6000) {
        erweitertesSetupWunsch = true;
        break;
      }
    }

    if (erweitertesSetupWunsch) {
      runExtendedSetup();
    } else {
      runSetupMode();
    }
  }

  playNormalBootAnimation();

  // Hardware-Timer 1 fuer 50Hz PWM initialisieren
  pinMode(PIN_SERVO1, OUTPUT);
  pinMode(PIN_SERVO2, OUTPUT);

  noInterrupts();
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Prescaler = 8
  ICR1 = 40000;                                        // 50 Hz Periode (20ms)
  OCR1A = 3000;                                        // Servo 1 Mitte (1.5ms)
  OCR1B = 3000;                                        // Servo 2 Mitte (1.5ms)
  interrupts();

  // Hardware-Serial starten
  Serial.begin(100000, SERIAL_8E2);
  lastByteTime = micros();
}

void loop() {
  while (Serial.available()) {
    uint8_t b = Serial.read();
    unsigned long nowMicros = micros();

    // Wenn im erweiterten Setup auf "Nicht-Invertiert" umgestellt wurde,
    // drehen wir das Byte per Software um. Im Standardbetrieb (1) bleibt b unangetastet.
    if (sbusInverted == 0) {
      b = ~b;
    }

    if (nowMicros - lastByteTime > 3000) {
      bufferIndex = 0;
    }
    lastByteTime = nowMicros;

    if (bufferIndex == 0 && b != 0x0F) {
      continue;
    }

    sbusBuffer[bufferIndex++] = b;

    if (bufferIndex == 25) {
      bufferIndex = 0;

      // Failsafe-Bit auswerten (Byte 23, Bit 4)
      bool failsafeActive = sbusBuffer[23] & (1 << 4);

      if (failsafeActive) {
        // Bei Failsafe die Ausgaenge trennen (Servos kraftlos schalten)
        TCCR1A &= ~((1 << COM1A1) | (1 << COM1B1));
      } else {
        // Ausgaenge wieder mit Timer verbinden
        TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

        parseSBUS();

        // Zeitstempel fuer Erstempfang setzen
        if (!sbusDetected) {
          sbusDetected = true;
          sbusFirstDetectTime = millis();
        }

        // SBUS-Werte (172-1811) in Timer-Ticks (2000-4000) umrechnen
        uint16_t pwm1 = map(sbusChannels[activeChannel1], 172, 1811, 2000, 4000);
        uint16_t pwm2 = map(sbusChannels[activeChannel2], 172, 1811, 2000, 4000);

        OCR1A = constrain(pwm1, 2000, 4000);
        OCR1B = constrain(pwm2, 2000, 4000);
      }
    }
  }

  // Asynchroner LED-Indikator fuer SBUS-Signal
  if (sbusDetected && !sbusAnimationDone) {
    unsigned long zeitSeitErkennung = millis() - sbusFirstDetectTime;
    if (zeitSeitErkennung >= 750 && zeitSeitErkennung < 1250) {
      if ((zeitSeitErkennung / 40) % 2 == 0) {
        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, HIGH);
      } else {
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, LOW);
      }
    } else if (zeitSeitErkennung >= 1250) {
      digitalWrite(PIN_LED1, LOW);
      digitalWrite(PIN_LED2, LOW);
      sbusAnimationDone = true;
    }
  }
}

void parseSBUS() {
  sbusChannels[0] = ((sbusBuffer[1] | sbusBuffer[2] << 8) & 0x07FF);
  sbusChannels[1] = ((sbusBuffer[2] >> 3 | sbusBuffer[3] << 5) & 0x07FF);
  sbusChannels[2] = ((sbusBuffer[3] >> 6 | sbusBuffer[4] << 2 | sbusBuffer[5] << 10) & 0x07FF);
  sbusChannels[3] = ((sbusBuffer[5] >> 1 | sbusBuffer[6] << 7) & 0x07FF);
  sbusChannels[4] = ((sbusBuffer[6] >> 4 | sbusBuffer[7] << 4) & 0x07FF);
  sbusChannels[5] = ((sbusBuffer[7] >> 7 | sbusBuffer[8] << 1 | sbusBuffer[9] << 9) & 0x07FF);
  sbusChannels[6] = ((sbusBuffer[9] >> 2 | sbusBuffer[10] << 6) & 0x07FF);
  sbusChannels[7] = ((sbusBuffer[10] >> 5 | sbusBuffer[11] << 3) & 0x07FF);
  sbusChannels[8] = ((sbusBuffer[12] | sbusBuffer[13] << 8) & 0x07FF);
  sbusChannels[9] = ((sbusBuffer[13] >> 3 | sbusBuffer[14] << 5) & 0x07FF);
  sbusChannels[10] = ((sbusBuffer[14] >> 6 | sbusBuffer[15] << 2 | sbusBuffer[16] << 10) & 0x07FF);
  sbusChannels[11] = ((sbusBuffer[16] >> 1 | sbusBuffer[17] << 7) & 0x07FF);
  sbusChannels[12] = ((sbusBuffer[17] >> 4 | sbusBuffer[18] << 4) & 0x07FF);
  sbusChannels[13] = ((sbusBuffer[18] >> 7 | sbusBuffer[19] << 1 | sbusBuffer[20] << 9) & 0x07FF);
  sbusChannels[14] = ((sbusBuffer[20] >> 2 | sbusBuffer[21] << 6) & 0x07FF);
  sbusChannels[15] = ((sbusBuffer[21] >> 5 | sbusBuffer[22] << 3) & 0x07FF);
}

void playNormalBootAnimation() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_LED2, HIGH);
    delay(250);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    delay(250);
  }
  delay(1000);
}
void blinkChannelInfo(int ledPin, uint8_t channelZeroIndexed) {
  uint8_t humanChannel = channelZeroIndexed + 1;
  for (int i = 0; i < humanChannel; i++) {
    digitalWrite(ledPin, HIGH);
    delay(150);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}
void runExtendedSetup() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_LED2, HIGH);
    delay(150);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    delay(150);
  }
  delay(1000);
  bool extendedSetupActive = true;
  unsigned long tasterDruckDauer = 0;
  while (extendedSetupActive) {
    if (sbusInverted == 1) {
      digitalWrite(PIN_LED1, HIGH);
      digitalWrite(PIN_LED2, HIGH);
      for (int t = 0; t < 85; t++) {
        if (digitalRead(PIN_SWITCH) == LOW) {
          tasterDruckDauer += 10;
        } else if (tasterDruckDauer > 0) {
          extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
          tasterDruckDauer = 0;
          break;
        }
        if (tasterDruckDauer >= 800) break;
        delay(10);
      }
      if (tasterDruckDauer >= 800) {
        extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
        tasterDruckDauer = 0;
        continue;
      }
      digitalWrite(PIN_LED1, LOW);
      digitalWrite(PIN_LED2, LOW);
      for (int t = 0; t < 15; t++) {
        if (extendedSetupActive == false) break;
        if (digitalRead(PIN_SWITCH) == LOW) {
          tasterDruckDauer += 10;
        } else if (tasterDruckDauer > 0) {
          extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
          tasterDruckDauer = 0;
          break;
        }
        if (tasterDruckDauer >= 800) break;
        delay(10);
      }
    } else {
      digitalWrite(PIN_LED1, HIGH);
      digitalWrite(PIN_LED2, HIGH);
      for (int t = 0; t < 15; t++) {
        if (digitalRead(PIN_SWITCH) == LOW) {
          tasterDruckDauer += 10;
        } else if (tasterDruckDauer > 0) {
          extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
          tasterDruckDauer = 0;
          break;
        }
        if (tasterDruckDauer >= 800) break;
        delay(10);
      }
      if (tasterDruckDauer >= 800) {
        extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
        tasterDruckDauer = 0;
        continue;
      }
      digitalWrite(PIN_LED1, LOW);
      digitalWrite(PIN_LED2, LOW);
      for (int t = 0; t < 85; t++) {
        if (extendedSetupActive == false) break;
        if (digitalRead(PIN_SWITCH) == LOW) {
          tasterDruckDauer += 10;
        } else if (tasterDruckDauer > 0) {
          extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
          tasterDruckDauer = 0;
          break;
        }
        if (tasterDruckDauer >= 800) break;
        delay(10);
      }
    }
    if (tasterDruckDauer >= 800) {
      extendedSetupActive = evaluateExtendedClick(tasterDruckDauer);
      tasterDruckDauer = 0;
    }
  }
}
bool evaluateExtendedClick(unsigned long dauer) {
  if (dauer < 800) {
    sbusInverted = (sbusInverted == 1) ? 0 : 1;
    return true;
  } else {
    EEPROM.update(EEPROM_ADDR_INVERT, sbusInverted);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    delay(200);
    for (int i = 0; i < 3; i++) {
      digitalWrite(PIN_LED1, HIGH);
      digitalWrite(PIN_LED2, HIGH);
      delay(300);
      digitalWrite(PIN_LED1, LOW);
      digitalWrite(PIN_LED2, LOW);
      delay(300);
    }
    delay(1000);
    return false;
  }
}
void runSetupMode() {
  bool setupComplete = false;
  while (!setupComplete) {
    while (digitalRead(PIN_SWITCH) == LOW) { delay(10); }
    delay(500);
    for (int i = 0; i < 3; i++) {
      digitalWrite(PIN_LED1, HIGH);
      digitalWrite(PIN_LED2, HIGH);
      delay(250);
      digitalWrite(PIN_LED1, LOW);
      digitalWrite(PIN_LED2, LOW);
      delay(250);
    }
    delay(1000);
    blinkChannelInfo(PIN_LED1, activeChannel1);
    delay(1500);
    blinkChannelInfo(PIN_LED2, activeChannel2);
    delay(2000);
    int selectedCh1 = recordChannel(PIN_LED1, PIN_LED2);
    if (selectedCh1 == -1) continue;
    for (int i = 0; i < 3; i++) {
      digitalWrite(PIN_LED1, HIGH);
      delay(300);
      digitalWrite(PIN_LED1, LOW);
      delay(300);
    }
    delay(1500);
    int selectedCh2 = recordChannel(PIN_LED2, PIN_LED1);
    if (selectedCh2 == -1) continue;
    for (int i = 0; i < 3; i++) {
      digitalWrite(PIN_LED2, HIGH);
      delay(333);
      digitalWrite(PIN_LED2, LOW);
      delay(333);
    }
    activeChannel1 = selectedCh1;
    activeChannel2 = selectedCh2;
    EEPROM.update(EEPROM_ADDR_CH1, activeChannel1);
    EEPROM.update(EEPROM_ADDR_CH2, activeChannel2);
    setupComplete = true;
    delay(2000);
  }
}
int recordChannel(int activeLed, int blinkingLed) {
  digitalWrite(activeLed, HIGH);
  delay(1000);
  for (int ch = 0; ch < 16; ch++) {
    digitalWrite(blinkingLed, HIGH);
    for (int t = 0; t < 15; t++) {
      if (digitalRead(PIN_SWITCH) == LOW) {
        digitalWrite(blinkingLed, LOW);
        digitalWrite(activeLed, LOW);
        return ch;
      }
      delay(10);
    }
    digitalWrite(blinkingLed, LOW);
    for (int t = 0; t < 50; t++) {
      if (digitalRead(PIN_SWITCH) == LOW) {
        digitalWrite(activeLed, LOW);
        return ch;
      }
      delay(10);
    }
    for (int t = 0; t < 100; t++) {
      if (digitalRead(PIN_SWITCH) == LOW) {
        digitalWrite(activeLed, LOW);
        return ch;
      }
      delay(10);
    }
  }
  digitalWrite(activeLed, LOW);
  return -1;
}
