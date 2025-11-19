#include <Arduino.h>
#include <SPI.h> 

#include "0_config.h"
#include "logger.h"
#include "led.h"
#include "lora.h" 
#include "interface.h" 


void setup() {
  Serial.begin(115200);

  // Initialisiere die SPI-Schnittstelle
  SPI.begin();

  // Der Start-Log sollte über den Logger erfolgen
  Serial.println("{\"level\":\"INFO\",\"message\":\"LoRa Node starting...\"}");

  setupLED();
  setupJsonSerial();
  setupLoRa();

  // NEU: Setze den LED-Modus basierend auf dem LoRa-Initialisierungsstatus
  if (isLoraReady()) {
    setHeartbeatMode(); // LoRa ist bereit, normaler Betrieb
    logMessage("INFO", "System ist bereit.");
  } else {
    setErrorMode();     // LoRa konnte nicht initialisiert werden, Fehler anzeigen
    logMessage("ERROR", "LoRa-Modul nicht bereit. System im Fehlermodus.");
  }
}

void loop() {
  handleLED();
  
  // LoRa-Funktionen nur ausführen, wenn das Modul bereit ist
  if (isLoraReady()) {
    if (receivedFlag) {
      checkLoRaReceived();
    }
  }
  
  handleJsonInput();
}