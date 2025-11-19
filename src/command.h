#ifndef COMMAND_H
#define COMMAND_H


/**
 * @brief Gibt eine Hilfe-Nachricht als String zurück.
 * @return String Eine formatierte Hilfe-Nachricht.
 */
String showHelp();

/**
 * @brief Gibt die aktuelle LoRa-Konfiguration als String zurück.
 * @return String Die aktuelle LoRa-Konfiguration.
 */
String getLoraConfig();

/**
 * @brief Verarbeitet eine Sendeanforderung für ein LoRa-Paket.
 *        Dekodiert den Base64-Payload und übergibt ihn an das LoRa-Modul.
 * 
 * @param base64Payload Der Base64-kodierte Payload als String.
 * @return String Eine Erfolgs- oder Fehlermeldung.
 */
String sendLoraPayload(const String& base64Payload);

/**
 * @brief Setzt die LoRa-Konfiguration des Moduls anhand der übergebenen (optionalen) Parameter.
 *        Fehlende (nicht-engagierte) optional-Parameter werden mit den aktuellen Einstellungen 
 *        aus dem LoRa-Modul aufgefüllt. Ruft intern setLoRaParameters im lora-Modul auf.
 * 
 * @param base_frequency_MHz Optionale neue Basisfrequenz in MHz.
 * @param frequency_offset_kHz Optionaler neuer Frequenzoffset in kHz.
 * @param bandwidth_kHz Optionale neue Bandbreite in kHz.
 * @param spreadingFactor Optionaler neuer Spreading Factor (SF).
 * @param codingRate Optionale neue Coding Rate (CR).
 * @param syncWord Optionales neues Synchronisationswort.
 * @param outputPower_dBm Optionale neue Sendeleistung in dBm.
 * @param preambleLength Optionale neue Präambellänge.
 * @return String Eine Erfolgs- oder Fehlermeldung.
 */
String setLoraConfig(std::optional<float> base_frequency_MHz, 
                     std::optional<float> frequency_offset_kHz, 
                     std::optional<float> bandwidth_kHz, 
                     std::optional<uint8_t> spreadingFactor, 
                     std::optional<uint8_t> codingRate, 
                     std::optional<uint8_t> syncWord, 
                     std::optional<int8_t> outputPower_dBm, 
                     std::optional<uint16_t> preambleLength);

#endif // COMMAND_H