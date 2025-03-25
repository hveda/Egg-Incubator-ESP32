#ifndef WS_CONFIG_H
#define WS_CONFIG_H

#include <Arduino.h>

// Incubator configuration structure
struct IncubatorConfig {
  char STASSID[32] = "heri.life";           // Default Wi-Fi SSID
  char STAPSK[64] = "monicacitra";        // Default Wi-Fi Password
  int WIFI_Enable = 1;                         // Wi-Fi Enable Flag (1: Enabled, 0: Disabled)
  int Extension_Enable = 1;                    // External Device Enable Flag
  int RTC_Enable = 0;                          // RTC Enable Flag
  float targetTemperature = 37.5;              // Target temperature in °C
  float targetHumidity = 55.0;                 // Target humidity in %
  float tempHysteresis = 0.2;                  // Temperature hysteresis in °C
  float humHysteresis = 5.0;                   // Humidity hysteresis in %
  int rollerInterval = 2;                      // Roller interval in hours
};

// Global configuration instance
extern IncubatorConfig config;

// Configuration management functions
void Config_Init();           // Initialize the configuration
void Config_Save();           // Save the configuration to SPIFFS
void Config_Load();           // Load the configuration from SPIFFS

#endif
