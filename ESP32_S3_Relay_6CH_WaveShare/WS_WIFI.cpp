#include "WS_WIFI.h"
#include "WS_Config.h"
#include "FS.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>  // mDNS library
#include <ArduinoJson.h>
#include "WS_Information.h"
const char *ssid = STASSID;                   // Name of the Wi-Fi you want to connect to
const char *password = STAPSK;                // The Wi-Fi password to connect to

WebServer server(80);                         // Declare the WebServer object
char ipStr[16];
bool WIFI_Connection = 0;

/******************************************* Serve File from SPIFFS ********************************************/
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "404: File Not Found");
    printf("Failed to open /index.html\r\n");
    return;
  }

  printf("Serving /index.html\r\n");
  server.streamFile(file, "text/html");
  file.close();
}

void handleGetData() {
  String json = "[";
  for (int i = 0; i < sizeof(Relay_Flag) / sizeof(Relay_Flag[0]); i++) {
    json += String(Relay_Flag[i]);
    if (i < sizeof(Relay_Flag) / sizeof(Relay_Flag[0]) - 1) {
      json += ",";
    }
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleSwitch(int ledNumber) {
  uint8_t Data[1] = {0};
  Data[0] = ledNumber + 48;
  Relay_Analysis(Data, WIFI_Mode);
  server.send(200, "text/plain", "OK");
}

void handleSwitch1() { handleSwitch(1); }
void handleSwitch2() { handleSwitch(2); }
void handleSwitch3() { handleSwitch(3); }
void handleSwitch4() { handleSwitch(4); }
void handleSwitch5() { handleSwitch(5); }
void handleSwitch6() { handleSwitch(6); }
void handleSwitch7() { handleSwitch(7); }
void handleSwitch8() { handleSwitch(8); }
void handleSaveConfig() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }

  String body = server.arg("plain");
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "text/plain", "Failed to parse JSON");
    return;
  }

  // Update configuration
  strlcpy(config.STASSID, doc["STASSID"] | config.STASSID, sizeof(config.STASSID));
  strlcpy(config.STAPSK, doc["STAPSK"] | config.STAPSK, sizeof(config.STAPSK));
  config.WIFI_Enable = doc["WIFI_Enable"] | config.WIFI_Enable;
  config.Extension_Enable = doc["Extension_Enable"] | config.Extension_Enable;
  config.RTC_Enable = doc["RTC_Enable"] | config.RTC_Enable;
  config.targetTemperature = doc["targetTemperature"] | config.targetTemperature;
  config.targetHumidity = doc["targetHumidity"] | config.targetHumidity;
  config.tempHysteresis = doc["tempHysteresis"] | config.tempHysteresis;
  config.humHysteresis = doc["humHysteresis"] | config.humHysteresis;
  config.rollerInterval = doc["rollerInterval"] | config.rollerInterval;

  Config_Save();  // Save updated configuration to SPIFFS
  server.send(200, "text/plain", "Configuration saved");
}

void handleGetConfig() {
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

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

/******************************************* Initialize SPIFFS, Wi-Fi, and mDNS ********************************************/
void WIFI_Init() {
  uint8_t Count = 0;

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    printf("Failed to mount SPIFFS\r\n");
    return;
  }
  printf("SPIFFS mounted successfully\r\n");

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(true);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Count++;
    delay(500);
    printf(".\r\n");
    if (Count % 2 == 0 && Count != 0) {
      RGB_Light(60, 0, 0);
      delay(1000);
      RGB_Light(0, 0, 0);
    }
    if (Count % 10 == 0) {
      WiFi.disconnect();
      delay(100);
      WiFi.mode(WIFI_OFF);
      delay(100);
      WiFi.mode(WIFI_STA);
      delay(100);
      WiFi.begin(ssid, password);
    }
    if (Count > 22) {  // connection fail
      break;
    }
  }
  delay(100);
  if (Count < 23) {
    WIFI_Connection = 1;
    RGB_Light(0, 60, 0);
    delay(1000);
    RGB_Light(0, 0, 0);
    IPAddress myIP = WiFi.localIP();
    printf("IP Address: ");
    sprintf(ipStr, "%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
    printf("%s\r\n", ipStr);

    // Register server routes
    server.on("/", handleRoot);
    server.on("/getData", handleGetData);
    server.on("/Switch1", handleSwitch1);
    server.on("/Switch2", handleSwitch2);
    server.on("/Switch3", handleSwitch3);
    server.on("/Switch4", handleSwitch4);
    server.on("/Switch5", handleSwitch5);
    server.on("/Switch6", handleSwitch6);
    server.on("/AllOn", handleSwitch7);
    server.on("/AllOff", handleSwitch8);

    server.begin();  // Start the server
    printf("Web server started\r\n");

    // Initialize mDNS
    if (!MDNS.begin("incubator")) {  // Use "incubator.local"
      printf("Error setting up mDNS responder\r\n");
    } else {
      printf("mDNS responder started at incubator.local\r\n");

      // Add an HTTP service to mDNS
      MDNS.addService("http", "tcp", 80);
    }
  } else {
    WIFI_Connection = 0;
    printf("Wi-Fi connection failed. You can use the Bluetooth debugging Assistant to control the device.\r\n");
    RGB_Light(60, 0, 0);
  }
}

void WIFI_Loop() {
  if (WIFI_Connection == 1) {
    server.handleClient();  // Processing requests from clients
    // MDNS.update();          // Update mDNS
  }
}
