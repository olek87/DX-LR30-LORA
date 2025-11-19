#include <Base64.h> 
#include <Arduino.h>

#include "codec.h"

void base64_encode(const uint8_t* data, size_t len, String& output) {
  // 1. Berechnen der benötigten Länge für die Base64-Ausgabe
  int encodedLen = Base64.encodedLength(len);

  // 2. Erstellen eines temporären Puffers für die Ausgabe
  char encodedChars[encodedLen + 1];

  // 3. Durchführen der Kodierung direkt mit uint8_t* Daten
  Base64.encode(encodedChars, (char*)data, len);
  
  // 4. Sicherstellen, dass das Ergebnis Null-terminiert ist
  encodedChars[encodedLen] = '\0';
  
  // 5. Zuweisen des Ergebnisses zum output-String
  output = String(encodedChars);
}

void base64_decode(const String& input, uint8_t* output, size_t& outputLen) {
  // Länge des Base64-Eingabe-Strings ermitteln
  size_t inputLength = input.length();
  
  // Erstelle eine mutable Kopie des Eingabe-Strings, da die Base64-Bibliothek
  // einen char* (nicht-konstant) für die Dekodierung erwartet.
  char mutableInputChars[inputLength + 1];
  input.toCharArray(mutableInputChars, inputLength + 1);

  // 1. Berechnen der benötigten Länge für den dekodierten String
  int decodedLen = Base64.decodedLength(mutableInputChars, inputLength);

  // 2. Durchführen der Dekodierung direkt in den output-Buffer
  Base64.decode((char*)output, mutableInputChars, inputLength);
  
  // 3. Länge zurückgeben
  outputLen = decodedLen;
}