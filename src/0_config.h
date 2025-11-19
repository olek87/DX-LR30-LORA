#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//================================================================================
// Logging Konfiguration
//================================================================================
#define DEFAULT_LOGGING_STATE true

//================================================================================
// Hardware Pin-Definitionen
//================================================================================
#define NSS PA4   // SX1262 Chip Select
#define DIO1 PC15 // SX1262 DIO1 (Interrupt-Pin)
#define NRST PA3  // SX1262 Reset-Pin
#define BUSY PA2  // SX1262 Busy-Pin
#define RXEN PA1  // RF Switch: RX Enable
#define TXEN PA0  // RF Switch: TX Enable

#define SX1262_TCXOVOLTAGE 0 // Spannung für den TCXO (falls vorhanden, sonst 0)

const int ledPin = PC13;    // Onboard-LED
const int buttonPin = PB12; // Onboard-Taster

//================================================================================
// LoRa Standard-Parameter (Europa 868 MHz)
//================================================================================
#define LORA_FREQUENCY 869.525 // Frequenz in MHz
#define LORA_BW 250.0          // Bandbreite in kHz (z.B. 125.0, 250.0)
#define LORA_SF 11             // Spreading Factor (7-12)
#define LORA_CR 5              // Coding Rate (5-8, entspricht 4/5 bis 4/8)
#define LORA_SYNC_WORD 0x1B    // Synchronisationswort
#define LORA_TX_POWER 22       // Sendeleistung in dBm (0-22)
#define LORA_PREAMBLE 16       // Länge der Präambel
#define LORA_FREQUENCY_OFFSET 10.5 // Frequenz-Offset in kHz zur Kompensation

#endif // CONFIG_H

// ======================================================================
// === Meshtastic Long Fast =====================================       
// ======================================================================
//#define LORA_FREQUENCY      869.525       // LoRa Frequenz in MHz
//#define LORA_BW             250.0       // LoRa Bandbreite in kHz (z.B. 125.0, 250.0, 500.0)
//#define LORA_SF             11           // LoRa Spreading Factor (z.B. 7, 8, 9, 10, 11, 12)
//#define LORA_CR             5           // LoRa Coding Rate (z.B. 5, 6, 7, 8)
//#define LORA_SYNC_WORD      0x1B        // LoRa Synchronisationswort (Standard: 0x12 für Public, 0x34 für Private)
//#define LORA_TX_POWER       22          // LoRa Sendeleistung in dBm (0 bis 22 dBm)
//#define LORA_PREAMBLE       16           // LoRa Präambellänge (Standard: 8 für SX126x)

// ======================================================================
// === MeshCore  =====================================       
// ======================================================================
//#define LORA_FREQUENCY      869.618       // LoRa Frequenz in MHz
//#define LORA_BW             62.5       // LoRa Bandbreite in kHz (z.B. 125.0, 250.0, 500.0)
//#define LORA_SF             8           // LoRa Spreading Factor (z.B. 7, 8, 9, 10, 11, 12)
//#define LORA_CR             8           // LoRa Coding Rate (z.B. 5, 6, 7, 8)
//#define LORA_SYNC_WORD      0x12        // LoRa Synchronisationswort (Standard: 0x12 für Public, 0x34 für Private)
//#define LORA_TX_POWER       22          // LoRa Sendeleistung in dBm (0 bis 22 dBm)
//#define LORA_PREAMBLE       16           // LoRa Präambellänge (Standard: 8 für SX126x)