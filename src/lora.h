#ifndef LORA_H
#define LORA_H

//================================================================================
// LoRa-Datenstruktur
//================================================================================

/**
 * @brief Enthält alle konfigurierbaren Parameter für das LoRa-Modul.
 */
struct LoRaSettings {
    float base_frequency_MHz;   // Basis-Trägerfrequenz in MHz
    float frequency_offset_kHz; // Frequenz-Offset in kHz
    float frequency_MHz;        // Resultierende Arbeitsfrequenz (Basis + Offset)
    float bandwidth_kHz;        // Kanalbandbreite in kHz
    uint8_t spreadingFactor;    // Spreading Factor (SF)
    uint8_t codingRate;         // Coding Rate (CR)
    uint8_t syncWord;           // Synchronisationswort
    int8_t outputPower_dBm;     // Sendeleistung in dBm
    uint16_t preambleLength;    // Länge der Präambel
};

//================================================================================
// Globale Interrupt-Flags
//================================================================================

/**
 * @brief Globales, volatiles Flag, das von einer ISR auf 'true' gesetzt wird,
 *        sobald ein LoRa-Paket vollständig empfangen wurde (RxDone-Interrupt).
 *        Muss in der Hauptschleife manuell zurückgesetzt werden.
 */
extern volatile bool receivedFlag;

//================================================================================
// Setup und Status
//================================================================================

/**
 * @brief Initialisiert das LoRa-Modul, konfiguriert die Pins, setzt die 
 *        initialen Parameter und startet den Empfangsmodus.
 */
void setupLoRa();

/**
 * @brief Prüft, ob das LoRa-Modul erfolgreich initialisiert wurde und bereit ist.
 * @return true, wenn das Modul bereit ist, ansonsten false.
 */
bool isLoraReady();

//================================================================================
// Laufzeit-Handler (für die Hauptschleife)
//================================================================================

/**
 * @brief Prüft das 'receivedFlag'. Wenn es gesetzt ist, wird das empfangene
 *        Paket ausgelesen, verarbeitet und über die Schnittstelle publiziert.
 *        Muss regelmäßig in der Hauptschleife aufgerufen werden.
 */
void checkLoRaReceived();


//================================================================================
// Konfiguration und Aktionen
//================================================================================

/**
 * @brief Sendet ein LoRa-Paket blockierend und wechselt danach wieder in den Empfangsmodus.
 *        Setzt das 'receivedFlag' zurück, um fälschliche Empfangs-Trigger durch TxDone zu vermeiden.
 * 
 * @param data Zeiger auf den Puffer mit den zu sendenden Daten.
 * @param len  Anzahl der zu sendenden Bytes.
 * @return Eine leere Zeichenkette bei Erfolg, andernfalls eine Fehlermeldung.
 */
String sendLoRaPacket(const uint8_t* data, size_t len);

/**
 * @brief Gibt eine Kopie der aktuell aktiven LoRa-Einstellungen zurück.
 * @return Eine 'LoRaSettings'-Struktur mit den aktuellen Werten.
 */
LoRaSettings getCurrentLoRaSettings();

/**
 * @brief Setzt neue LoRa-Parameter, wendet sie auf das Modul an und speichert sie bei Erfolg.
 *        Aktualisiert die internen Einstellungen nur bei erfolgreicher Anwendung auf der Hardware.
 * 
 * @param base_frequency_MHz   Die neue Basisfrequenz in MHz.
 * @param frequency_offset_kHz Der neue Frequenzoffset in kHz.
 * @param bandwidth_kHz        Die neue Bandbreite in kHz.
 * @param spreadingFactor      Der neue Spreading Factor (SF).
 * @param codingRate           Die neue Coding Rate (CR).
 * @param syncWord             Das neue Synchronisationswort.
 * @param outputPower_dBm      Die neue Sendeleistung in dBm.
 * @param preambleLength       Die neue Präambellänge.
 * @return String Eine Erfolgs- ("INFO: ...") oder Fehlermeldung ("ERROR: ...").
 */
String setLoRaParameters(float base_frequency_MHz, float frequency_offset_kHz, float bandwidth_kHz, 
                         uint8_t spreadingFactor, uint8_t codingRate, uint8_t syncWord, 
                         int8_t outputPower_dBm, uint16_t preambleLength);

#endif // LORA_H
