#include <Arduino.h>

#include "logger.h"
#include "0_config.h"
#include "interface.h" 

// Variable zur Steuerung des Logging-Status zur Laufzeit
// Initialisiert mit dem Wert aus der Konfigurationsdatei
static bool loggingActive = DEFAULT_LOGGING_STATE;

void logMessage(const char* level, const String& message) {
    // Prüfen, ob das Logging zur Laufzeit aktiv ist
    if (!loggingActive) {
        return; // Logging ist deaktiviert, also nichts tun
    }

    // Übergib die Log-Daten an das Interface zur JSON-Formatierung und Ausgabe
    publishLogAsJson(level, message);
    Serial.println(); // Neue Zeile nach der JSON-Ausgabe
}

void setLogging(bool enabled) {
    if (loggingActive != enabled) {
        loggingActive = enabled;
        // Die Status-Nachricht wird immer gesendet, um die Änderung zu bestätigen.
        // Das Interface kümmert sich um die JSON-Formatierung.
        publishLogAsJson("STATUS", "Logging " + String(enabled ? "aktiviert" : "deaktiviert"));
    }
}

bool isLoggingEnabled() {
    return loggingActive;
}