#include "ws_config.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Global configuration instance
IncubatorConfig config;

void Config_Init() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
  if (!SPIFFS.exists("/config.json")) {
    Serial.println("Config file does not exist.");
    Config_Save();  // Create a default config file
  }

  Config_Load();
}

void Config_Save() {
  File file = SPIFFS.open("/config.json", "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
    return;
  }

  StaticJsonDocument<256> doc;
  doc["STASSID"] = config.STASSID;
  doc["STAPSK"] = config.STAPSK;
  doc["WIFI_Enable"] = config.WIFI_Enable;
  doc["Extension_Enable"] = config.Extension_Enable;
  doc["RTC_Enable"] = config.RTC_Enable;
  doc["targetTemperature"] = config.targetTemperature;
  doc["targetHumidity"] = config.targetHumidity;
  doc["tempHysteresis"] = config.tempHysteresis;
  doc["humHysteresis"] = config.humHysteresis;
  doc["rollerInterval"] = config.rollerInterval;

  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to config file");
  } else {
    Serial.println("Configuration saved successfully");
  }
  file.close();
}

void Config_Load() {
  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    Serial.println("Config file not found, using defaults");
    Config_Save();  // Save default values to create a new file
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse config file, using defaults");
    Config_Save();  // Save default values to correct file corruption
    return;
  }

  // Load values from JSON or use defaults
  strlcpy(config.STASSID, doc["STASSID"] | "defaultSSID", sizeof(config.STASSID));
  strlcpy(config.STAPSK, doc["STAPSK"] | "defaultPassword", sizeof(config.STAPSK));
  config.WIFI_Enable = doc["WIFI_Enable"] | 1;
  config.Extension_Enable = doc["Extension_Enable"] | 0;
  config.RTC_Enable = doc["RTC_Enable"] | 1;
  config.targetTemperature = doc["targetTemperature"] | 25;
  config.targetHumidity = doc["targetHumidity"] | 50;
  config.tempHysteresis = doc["tempHysteresis"] | 2;
  config.humHysteresis = doc["humHysteresis"] | 5;
  config.rollerInterval = doc["rollerInterval"] | 30;
  Serial.println("Loading configuration...");

  // After deserializing JSON, add debugging:
  Serial.print("STASSID: ");
  Serial.println(config.STASSID);
  Serial.print("WIFI_Enable: ");
  Serial.println(config.WIFI_Enable);
  // Add other debug prints as needed for all fields

  Serial.println("Configuration loaded successfully");
  file.close();
}
