#include <Arduino.h> 
#include <optional>

#include "command.h"
#include "lora.h"    
#include "codec.h"   

String showHelp() {
    String helpText = "DX-LR30-LORA Hilfe: ";
    helpText += "Eingabe als JSON-Objekt mit Hauptschlüssel 'command'. ";
    helpText += "Verfügbare Befehle: ";
    helpText += "'help' - Zeigt diese Hilfe an. Bsp: {'command':{'help':{}}} ";
    helpText += "'getLoraConfig' - Zeigt aktuelle LoRa-Konfiguration an. Bsp: {'command':{'getLoraConfig':{}}} ";
    helpText += "'sendLora' - Sendet Base64-kodierte Daten. Bsp: {'command':{'sendLora':{'payload':'...Hallo...'}}} ";
    helpText += "'reset' - Führt einen Software-Reset des Geräts durch. Bsp: {'command':{'reset':{}}} ";
    helpText += "'setLoraConfig' - Setzt LoRa-Parameter (partiell möglich). Bsp: {'command':{'setLoraConfig':{'Freq':869.618, 'SF':8, 'CR':8, 'BW':62.5, 'Sync': '0x12', 'Offset': 10.3, 'Preamble': 16, 'Power': 21  }}}  ";

    return helpText;
}

String resetDevice() {
    // NVIC_SystemReset() löst den Reset sofort aus.
    // Der Rückgabewert wird wahrscheinlich nie verwendet, aber wir behalten ihn für die Konsistenz bei.
    NVIC_SystemReset(); 
    return "Gerät wird zurückgesetzt..."; // Diese Zeile wird nicht erreicht.
}

String getLoraConfig() {
    // 1. Die aktuellen Einstellungen aus dem lora-Modul abrufen
    LoRaSettings settings = getCurrentLoRaSettings();

    // 2. Die Einstellungen in einen kompakten, einzeiligen String umwandeln
    String configText = "LoRa Config: ";
    configText += "Freq=" + String(settings.base_frequency_MHz, 3) + " MHz, ";
    configText += "BW=" + String(settings.bandwidth_kHz) + " kHz, ";
    configText += "SF=" + String(settings.spreadingFactor) + ", ";
    configText += "CR=" + String(settings.codingRate) + ", ";
    configText += "Sync=0x" + String(settings.syncWord, HEX) + ", ";
    configText += "Power=" + String(settings.outputPower_dBm) + " dBm, ";
    configText += "Preamble=" + String(settings.preambleLength) + ", ";
    configText += "Offset=" + String(settings.frequency_offset_kHz, 1) + " kHz";
    
    return configText;
}

String sendLoraPayload(const String& base64Payload) {
    if (base64Payload.length() == 0) {
        // Leere Payloads sind nicht zulässig.
        return "ERROR: Leerer Base64-Payload empfangen.";
    }

    // Puffer für die dekodierten Daten. Ein LoRa-Paket kann maximal 255 Bytes enthalten.
    // Wir verwenden 256, um sicherzustellen, dass genügend Platz vorhanden ist.
    uint8_t decoded_payload[256];
    size_t decoded_len; // Variable zur Speicherung der tatsächlichen Länge der dekodierten Daten

    // Rufe die base64_decode-Funktion aus dem codec-Modul auf.
    // Diese Funktion füllt den 'decoded_payload'-Puffer und setzt 'decoded_len'.
    base64_decode(base64Payload, decoded_payload, decoded_len);

     // Prüfen, ob die Dekodierung erfolgreich war und die Länge im zulässigen Bereich liegt.
    if (decoded_len > 255) {
        // Falls die dekodierten Daten zu lang für ein einzelnes LoRa-Paket sind.
        return "ERROR: Payload nach Dekodierung zu lang (" + String(decoded_len) + " > 255 Bytes). Max 255 Bytes erlaubt.";
    } else {
        // Die dekodierten Daten sind gültig (Länge > 0 und <= 255).
        // Rufe die sendLoRaPacket-Funktion aus dem lora-Modul auf und verarbeite das Ergebnis.
        String loraSendResult = sendLoRaPacket(decoded_payload, decoded_len);

        if (loraSendResult.length() > 0) { // sendLoRaPacket hat eine Fehlermeldung zurückgegeben
            return "ERROR: " + loraSendResult; // Die Fehlermeldung aus lora.cpp weitergeben
        } else {
            // sendLoRaPacket hat einen leeren String zurückgegeben (Erfolg)
            return "LoRa-Paket erfolgreich gesendet. ";
        }
    }
}

String setLoraConfig(std::optional<float> base_frequency_MHz, 
                     std::optional<float> frequency_offset_kHz, 
                     std::optional<float> bandwidth_kHz, 
                     std::optional<uint8_t> spreadingFactor, 
                     std::optional<uint8_t> codingRate, 
                     std::optional<uint8_t> syncWord, 
                     std::optional<int8_t> outputPower_dBm, 
                     std::optional<uint16_t> preambleLength) {
    
    // 1. Aktuelle LoRa-Einstellungen abrufen (als Basiswerte)
    LoRaSettings current = getCurrentLoRaSettings();

    // 2. Temporäre Variablen mit den aktuellen Einstellungen als Standardwerten füllen
    //    und mit den optionalen neuen Werten überschreiben, falls vorhanden.
    float final_base_frequency_MHz   = base_frequency_MHz.value_or(current.base_frequency_MHz);
    float final_frequency_offset_kHz = frequency_offset_kHz.value_or(current.frequency_offset_kHz);
    float final_bandwidth_kHz        = bandwidth_kHz.value_or(current.bandwidth_kHz);
    uint8_t final_spreadingFactor    = spreadingFactor.value_or(current.spreadingFactor);
    uint8_t final_codingRate         = codingRate.value_or(current.codingRate);
    uint8_t final_syncWord           = syncWord.value_or(current.syncWord);
    int8_t final_outputPower_dBm     = outputPower_dBm.value_or(current.outputPower_dBm);
    uint16_t final_preambleLength    = preambleLength.value_or(current.preambleLength);

    // 3. Die vollständigen (aktualisierten) Parameter an das lora-Modul senden
    String result = setLoRaParameters(final_base_frequency_MHz, final_frequency_offset_kHz, final_bandwidth_kHz, 
                                  final_spreadingFactor, final_codingRate, final_syncWord, 
                                  final_outputPower_dBm, final_preambleLength);

    return result; // Das String-Ergebnis direkt zurückgeben
}