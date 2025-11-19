#include <Arduino.h>
#include <ArduinoJson.h>
#include <optional>

#include "interface.h"
#include "codec.h"  
#include "lora.h"   
#include "command.h"

// Buffer für eingehende serielle Daten
String jsonInputBuffer;

// Maximale Größe des JSON-Dokuments
const int JSON_DOC_SIZE_RX = 512; 
const int JSON_DOC_SIZE_LOG = 1024; 

void setupJsonSerial() {
    publishLogAsJson("INFO", "DX-LR30-LORA - JSON Interface initialisiert."); 
}

void publishLogAsJson(const char* level, const String& message) {
    StaticJsonDocument<JSON_DOC_SIZE_LOG> doc;
    doc["type"] = "log";
    doc["level"] = level;
    doc["message"] = message;
    
    serializeJson(doc, Serial);
    Serial.println(); 
}

void handleJsonInput() {
    while (Serial.available()) {
        char c = Serial.read();

        Serial.write(c);

        if (c == '\n' || c == '\r') {
            if (jsonInputBuffer.length() > 0) {
                Serial.println();
                // NEU: Konvertiere die gesamte Eingabe in Kleinbuchstaben, bevor irgendetwas anderes passiert.
                jsonInputBuffer.toLowerCase();

                // Prüfe auf den einfachen Befehl "help"
                if (jsonInputBuffer == "help") { // Einfacher Vergleich, da jetzt alles klein ist
                    publishLogAsJson("INFO", showHelp());
                } else {
                    StaticJsonDocument<JSON_DOC_SIZE_RX> doc;
                    DeserializationError error = deserializeJson(doc, jsonInputBuffer);

                    if (error == DeserializationError::Ok) {
                        // Hauptschlüssel 'command' muss jetzt klein sein
                        if (doc.containsKey("command") && doc["command"].is<JsonObject>()) {
                            JsonObject commandObj = doc["command"].as<JsonObject>();
                            String result = "";

                            // Unterbefehle prüfen (alle in Kleinbuchstaben)
                            if (commandObj.containsKey("getloraconfig")) {
                                result = getLoraConfig();
                                publishLogAsJson("INFO", result);
                            } else if (commandObj.containsKey("help")) {
                                result = showHelp();
                                publishLogAsJson("INFO", result);
                            } else if (commandObj.containsKey("sendlora")) {
                                JsonObject sendLoraObj = commandObj["sendlora"].as<JsonObject>();
                                if (sendLoraObj.containsKey("payload") && sendLoraObj["payload"].is<const char*>()) {
                                    String payload = sendLoraObj["payload"].as<String>();
                                    result = sendLoraPayload(payload);
                                    publishLogAsJson("INFO", "Befehl 'sendLora' ausgeführt: LoRa-Paket erfolgreich gesendet." );
                                } else {
                                    publishLogAsJson("ERROR", "Befehl 'sendlora' ohne gültigen 'payload'-String.");
                                }
                            } else if (commandObj.containsKey("setloraconfig")) {
                                JsonObject setLoraConfigObj = commandObj["setloraconfig"].as<JsonObject>();
                                
                                // Parameter für setLoraConfig (alle in Kleinbuchstaben)
                                std::optional<float> baseFreq;
                                if (setLoraConfigObj.containsKey("freq") && setLoraConfigObj["freq"].is<float>()) baseFreq = setLoraConfigObj["freq"].as<float>();
                                
                                std::optional<float> offset;
                                if (setLoraConfigObj.containsKey("offset") && setLoraConfigObj["offset"].is<float>()) offset = setLoraConfigObj["offset"].as<float>();
                                
                                std::optional<float> bw;
                                if (setLoraConfigObj.containsKey("bw") && setLoraConfigObj["bw"].is<float>()) bw = setLoraConfigObj["bw"].as<float>();
                                
                                std::optional<uint8_t> sf;
                                if (setLoraConfigObj.containsKey("sf") && setLoraConfigObj["sf"].is<uint8_t>()) sf = setLoraConfigObj["sf"].as<uint8_t>();
                                
                                std::optional<uint8_t> cr;
                                if (setLoraConfigObj.containsKey("cr") && setLoraConfigObj["cr"].is<uint8_t>()) cr = setLoraConfigObj["cr"].as<uint8_t>();
                                
                                std::optional<uint8_t> sync;
                                if (setLoraConfigObj.containsKey("sync")) {
                                    if (setLoraConfigObj["sync"].is<const char*>()) {
                                        const char* syncStr = setLoraConfigObj["sync"].as<const char*>();
                                        sync = (uint8_t)strtoul(syncStr, NULL, 0);
                                    } else if (setLoraConfigObj["sync"].is<uint8_t>()) {
                                        sync = setLoraConfigObj["sync"].as<uint8_t>();
                                    }
                                }
                                
                                std::optional<int8_t> power;
                                if (setLoraConfigObj.containsKey("power") && setLoraConfigObj["power"].is<int8_t>()) power = setLoraConfigObj["power"].as<int8_t>();
                                
                                std::optional<uint16_t> preamble;
                                if (setLoraConfigObj.containsKey("preamble") && setLoraConfigObj["preamble"].is<uint16_t>()) preamble = setLoraConfigObj["preamble"].as<uint16_t>();

                                result = setLoraConfig(baseFreq, offset, bw, sf, cr, sync, power, preamble);
                                publishLogAsJson("INFO", "Befehl 'setLoraConfig' ausgeführt: " + result); 
                            } else {
                                publishLogAsJson("WARN", "Unbekannter Befehlstyp im 'command'-Objekt.");
                            }
                        } else {
                            publishLogAsJson("WARN", "JSON ohne Hauptschlüssel 'command' empfangen.");
                        }
                    } else {
                        publishLogAsJson("ERROR", "JSON Deserialisierungsfehler: " + String(error.c_str()));
                    }
                }
                jsonInputBuffer = "";
            }
        } else {
            jsonInputBuffer += c;
        }
    }
}

void publishReceivedLoRaPacket(const uint8_t* payload, size_t len, int16_t rssi, float snr, float frequencyError) {
  StaticJsonDocument<JSON_DOC_SIZE_RX> doc;

  doc["type"] = "lora_rx";
  doc["rssi"] = rssi;
  doc["snr"] = snr;
  doc["frequencyError"] = round((frequencyError / 1000.0) * 100.0) / 100.0;

  String base64Payload;
  base64_encode(payload, len, base64Payload); 
  doc["payload"] = base64Payload;

  serializeJson(doc, Serial);
  Serial.println(); 
}

