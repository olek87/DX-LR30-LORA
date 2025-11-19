#ifndef INTERFACE_H
#define INTERFACE_H

// Aktuelle Funktion für den Empfang von LoRa-Paketen
void publishReceivedLoRaPacket(const uint8_t* payload, size_t len, int16_t rssi, float snr, float frequencyError);

// Neue Funktion zur Veröffentlichung von Log-Nachrichten als JSON
void publishLogAsJson(const char* level, const String& message);

// Funktionen für die serielle JSON-Kommunikation
void setupJsonSerial(); // NEU: Deklaration hinzugefügt
void handleJsonInput(); // NEU: Deklaration hinzugefügt

#endif // INTERFACE_H
