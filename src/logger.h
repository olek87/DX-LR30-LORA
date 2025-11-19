#ifndef LOGGER_H
#define LOGGER_H

/**
 * @brief Sendet eine formatierte JSON-Log-Nachricht über die serielle Schnittstelle.
 *        Die Ausgabe erfolgt nur, wenn das Logging aktiv ist.
 * 
 * @param level Die Log-Ebene (z.B. "INFO", "ERROR", "DEBUG").
 * @param message Die zu protokollierende Nachricht.
 */
void logMessage(const char* level, const String& message);

/**
 * @brief Aktiviert oder deaktiviert das Logging zur Laufzeit.
 * 
 * @param enabled true, um das Logging zu aktivieren, false, um es zu deaktivieren.
 */
void setLogging(bool enabled);

/**
 * @brief Gibt zurück, ob das Logging aktuell aktiv ist.
 * 
 * @return true, wenn das Logging aktiv ist, andernfalls false.
 */
bool isLoggingEnabled();

#endif // LOGGER_H