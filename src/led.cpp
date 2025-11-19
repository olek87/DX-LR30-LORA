#include <Arduino.h>

#include "led.h"
#include "0_config.h"

enum LedMode {
  LED_INIT,
  LED_HEARTBEAT,
  LED_RX_PULSE,
  LED_TX_PULSE,
  LED_ERROR
};

LedMode ledMode = LED_INIT;
unsigned long ledTimer = 0;
bool ledState = false;
int pulseCount = 0;

void setupLED() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void handleLED() {
  unsigned long currentTime = millis();

  switch (ledMode) {
    case LED_INIT:
      if (currentTime - ledTimer >= 200) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? HIGH : LOW);
        ledTimer = currentTime;
      }
      break;

    case LED_HEARTBEAT: {
      unsigned long elapsed = currentTime - ledTimer;
      if (elapsed < 2000) {
        digitalWrite(ledPin, LOW);
      } else if (elapsed < 2100) {
        digitalWrite(ledPin, HIGH);
      } else if (elapsed < 2200) {
        digitalWrite(ledPin, LOW);
      } else if (elapsed < 2300) {
        digitalWrite(ledPin, HIGH);
      } else {
        ledTimer = currentTime;
      }
      break;
    }

    case LED_RX_PULSE:
      if (pulseCount < 10) { // 5 Blitze = 10 Zustandsänderungen
        if (currentTime - ledTimer >= 50) { // 50ms an/aus
          ledState = !ledState;
          digitalWrite(ledPin, ledState ? HIGH : LOW);
          ledTimer = currentTime;
          pulseCount++;
        }
      } else {
        setHeartbeatMode(); // Zurück zum Heartbeat
      }
      break;

    case LED_TX_PULSE:
      // Ein einzelner 500ms Puls
      if (currentTime - ledTimer > 500) {
        digitalWrite(ledPin, LOW);
        setHeartbeatMode();
      }
      break;

    case LED_ERROR:
      if (currentTime - ledTimer >= 500) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? HIGH : LOW);
        ledTimer = currentTime;
      }
      break;
  }
}

void triggerRxPulse() {
  ledMode = LED_RX_PULSE;
  ledTimer = millis();
  pulseCount = 0;
  ledState = false;
}

void triggerTxPulse() {
  ledMode = LED_TX_PULSE;
  ledTimer = millis();
  digitalWrite(ledPin, HIGH);
}

void setErrorMode() {
  ledMode = LED_ERROR;
  ledTimer = millis();
}

void setHeartbeatMode() {
  ledMode = LED_HEARTBEAT;
  ledTimer = millis();
}