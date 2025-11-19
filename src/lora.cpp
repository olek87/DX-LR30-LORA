#include <Arduino.h>
#include <RadioLib.h>     

#include "0_config.h"    
#include "lora.h" 
#include "interface.h" 
#include "logger.h"
#include "led.h"

// Globale, statische Variable zur Speicherung der aktuellen LoRa-Einstellungen
static LoRaSettings currentLoRaSettings;

// Statusvariable, die anzeigt, ob das Modul einsatzbereit ist
static bool loraReady = false;

// Definition der globalen Flag-Variable für den Empfang (muss hier sein)
volatile bool receivedFlag = false;

// Erstellen Sie eine Instanz der RadioLib LoRa-Klasse
SX1262 radio = new Module(NSS, DIO1, NRST, BUSY); 

// Der Puffer zum Speichern der empfangenen Daten
uint8_t lora_packet_buffer[256];

LoRaSettings getCurrentLoRaSettings() {
    return currentLoRaSettings;
}

// Implementierung der Status-Funktion
bool isLoraReady() {
    return loraReady;
}

// ISR-Handler: Wird vom DIO1-Interrupt aufgerufen
void setFlag(void) {
  receivedFlag = true;
}

void setupLoRa() {
  logMessage("INFO", "Initialisiere LoRa-Modul...");

  // 1. Initiales Laden der Parameter aus der Konfigurationsdatei in die aktuelle Einstellung
  currentLoRaSettings.base_frequency_MHz   = LORA_FREQUENCY;
  currentLoRaSettings.frequency_offset_kHz = LORA_FREQUENCY_OFFSET; 
  currentLoRaSettings.frequency_MHz        = LORA_FREQUENCY + (LORA_FREQUENCY_OFFSET / 1000.0);
  currentLoRaSettings.bandwidth_kHz        = LORA_BW;
  currentLoRaSettings.spreadingFactor      = LORA_SF;
  currentLoRaSettings.codingRate           = LORA_CR;
  currentLoRaSettings.syncWord             = LORA_SYNC_WORD;
  currentLoRaSettings.outputPower_dBm      = LORA_TX_POWER;
  currentLoRaSettings.preambleLength       = LORA_PREAMBLE;

  // 2. Modul mit den geladenen Parametern initialisieren
  int state = radio.begin(currentLoRaSettings.frequency_MHz,
                          currentLoRaSettings.bandwidth_kHz,
                          currentLoRaSettings.spreadingFactor,
                          currentLoRaSettings.codingRate,
                          currentLoRaSettings.syncWord,
                          currentLoRaSettings.outputPower_dBm,
                          currentLoRaSettings.preambleLength, 
                          SX1262_TCXOVOLTAGE, false); 
  if (state != RADIOLIB_ERR_NONE) {
    logMessage("ERROR", "LoRa-Modul Initialisierung (radio.begin) fehlgeschlagen, Code: " + String(state));
    loraReady = false;
    setErrorMode(); // NEU: Fehler-LED aktivieren
    return;
  }

  // 3. RF-Schalter-Pins konfigurieren. Diese Funktion gibt KEINEN Statuscode zurück.
  radio.setRfSwitchPins(RXEN, TXEN); 
  logMessage("INFO", "RF-Schalter-Pins konfiguriert."); 

  // 4. Interrupt konfigurieren (NEU!) - wichtig für interrupt-basierten Empfang
  radio.setPacketReceivedAction(setFlag);

  // 5. Empfang starten (nach dem das Modul bereit ist und Interrupt konfiguriert wurde)
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    logMessage("ERROR", "Fehler beim Starten des Empfangsmodus: " + String(state));
    loraReady = false;
    setErrorMode(); // NEU: Fehler-LED aktivieren
    return;
  }

  logMessage("INFO", "LoRa-Modul ist bereit und im Empfangsmodus.");
  loraReady = true;
}
  
void checkLoRaReceived() {
  
  byte byteArr[256];
  int numBytes = radio.getPacketLength();
  
  if (numBytes > 0 && numBytes <= 256) {
    int state = radio.readData(byteArr, numBytes);
    
    if (state == RADIOLIB_ERR_NONE) {
      // Paket wurde erfolgreich empfangen
      triggerRxPulse(); // RX-Puls auslösen


      int16_t rssi = radio.getRSSI();
      float snr = radio.getSNR();
      float frequencyError = radio.getFrequencyError();

      publishReceivedLoRaPacket(byteArr, numBytes, rssi, snr, frequencyError); 

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // Paket wurde empfangen, aber ist fehlerhaft (CRC-Fehler)
      logMessage("WARN", "LoRa-Paket empfangen, aber CRC-Fehler!");
      // Optional: setErrorMode() wenn CRC-Fehler als kritisch angesehen werden
    } else if (state < 0) {
      // Einige andere Fehler sind aufgetreten
      logMessage("WARN", "LoRa-Paket empfangen, aber Empfangsfehler, Code: " + String(state));
    }
  } else {
    logMessage("ERROR", "LoRa-Paket empfangen, ungültige Paketlänge: " + String(numBytes));
  }

  // Wichtig: Nach der Bearbeitung des Pakets das Modul wieder in den Empfangsmodus versetzen,
  // um auf das nächste Paket zu warten.

  receivedFlag = false; // Flag zurücksetzen

  int startRxState = radio.startReceive();
  if (startRxState != RADIOLIB_ERR_NONE) {
    logMessage("ERROR", "Fehler beim Neustarten des Empfangs nach Paketbearbeitung: " + String(startRxState));
    setErrorMode(); // Fehler-LED aktivieren
  }
}

String sendLoRaPacket(const uint8_t* data, size_t len) {
  //logMessage("INFO", "Sende LoRa-Paket..."); // Log-Ausgabe hier entfernt, wird als Rückgabewert behandelt

  // Sende die Daten blockierend
  int state = radio.transmit(data, len);
  
  // WICHTIG: Den durch TxDone ausgelösten Interrupt sofort bereinigen,
  // da er sonst checkLoRaReceived() fälschlicherweise triggern würde.
  receivedFlag = false;

  triggerTxPulse(); // TX-Puls auslösen

  String resultMessage = ""; // Standardmäßig leer (Erfolg)

  if (state == RADIOLIB_ERR_NONE) {
    // Senden erfolgreich
    // triggerTxPulse(); // TX-LED-Puls auslösen
    // resultMessage bleibt leer
  } else {
    // Senden fehlgeschlagen
    resultMessage = "LoRa-Senden fehlgeschlagen, Code: " + String(state);
    setErrorMode(); // Fehler-LED aktivieren
  }
  
  // Nach dem Senden immer wieder in den Empfangsmodus wechseln
  int startRxState = radio.startReceive();
  if (startRxState != RADIOLIB_ERR_NONE) {
    String rxErrorMessage = "Fehler beim Neustarten des Empfangsmodus nach Senden: " + String(startRxState);
    setErrorMode(); // Fehler-LED aktivieren
    
    // Wenn schon ein Sendefehler vorlag, beide Fehler kombinieren
    if (resultMessage.length() > 0) { // <--- Hier geändert
      resultMessage += "; " + rxErrorMessage;
    } else {
      resultMessage = rxErrorMessage; // Nur Neustart-Fehler
    }
  }
  return resultMessage; // Gibt leeren String bei Erfolg oder eine Fehlermeldung zurück
}

// Diese Funktion ist jetzt 'static' und wird nur intern verwendet.
// Sie wendet die berechneten Werte direkt auf das Modul an.
static int applyLoRaRadioSettings(float frequency_MHz, float bandwidth_kHz, uint8_t spreadingFactor, 
                                  uint8_t codingRate, uint8_t syncWord, int8_t outputPower_dBm, 
                                  uint16_t preambleLength) {
  int state;
  // 1. Modul in den Standby-Modus versetzen
  state = radio.standby();
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Wechsel in Standby-Modus: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  
  // 2. Neue Parameter auf das Modul anwenden und Fehler prüfen
  state = radio.setFrequency(frequency_MHz); 
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen der Frequenz: " + String(frequency_MHz) + " MHz, Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setBandwidth(bandwidth_kHz); 
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen der Bandbreite: " + String(bandwidth_kHz) + " kHz, Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setSpreadingFactor(spreadingFactor);
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen des Spreading Factors: " + String(spreadingFactor) + ", Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setCodingRate(codingRate);
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen der Coding Rate: " + String(codingRate) + ", Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setSyncWord(syncWord); 
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen des Sync Word: " + String(syncWord, HEX) + ", Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setOutputPower(outputPower_dBm); 
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen der Sendeleistung: " + String(outputPower_dBm) + " dBm, Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }
  state = radio.setPreambleLength(preambleLength); 
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Setzen der Präambellänge: " + String(preambleLength) + ", Code: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }

  // 3. Modul wieder in den Empfangsmodus versetzen
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
      logMessage("ERROR", "Fehler beim Starten des Empfangs nach Parameteränderung: " + String(state));
      setErrorMode(); // NEU: Fehler-LED aktivieren
      return state;
  }

  //logMessage("INFO", "LoRa-Funkparameter erfolgreich auf Modul angewendet.");
  return RADIOLIB_ERR_NONE; // Alles erfolgreich
}

String setLoRaParameters(float base_frequency_MHz, float frequency_offset_kHz, float bandwidth_kHz, 
                      uint8_t spreadingFactor, uint8_t codingRate, uint8_t syncWord, 
                      int8_t outputPower_dBm, uint16_t preambleLength) {

  // Berechne die tatsächliche Arbeitsfrequenz
  float frequency_MHz = base_frequency_MHz + (frequency_offset_kHz / 1000.0);

  // Rufe die interne Funktion auf, um die Parameter auf die Hardware anzuwenden
  int state = applyLoRaRadioSettings(frequency_MHz, bandwidth_kHz, spreadingFactor, codingRate, 
                                     syncWord, outputPower_dBm, preambleLength);

          

  // Nur wenn das Anwenden erfolgreich war, aktualisieren wir unsere globale Konfiguration
  if (state == RADIOLIB_ERR_NONE) {
    currentLoRaSettings.base_frequency_MHz   = base_frequency_MHz;
    currentLoRaSettings.frequency_offset_kHz = frequency_offset_kHz;
    currentLoRaSettings.frequency_MHz        = frequency_MHz;
    currentLoRaSettings.bandwidth_kHz        = bandwidth_kHz;
    currentLoRaSettings.spreadingFactor      = spreadingFactor;
    currentLoRaSettings.codingRate           = codingRate;
    currentLoRaSettings.syncWord             = syncWord;
    currentLoRaSettings.outputPower_dBm      = outputPower_dBm;
    currentLoRaSettings.preambleLength       = preambleLength;
    return "INFO: LoRa-Konfiguration erfolgreich angewendet."; // Erfolgsmeldung zurückgeben
  } else {
    return "ERROR: LoRa-Konfiguration konnte nicht angewendet werden."; 
  }
}



