#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// BME280 configuration
Adafruit_BME280 bme;
#define BME280_I2C_ADDRESS 0x76

// DS18B20 configuration
#define ONE_WIRE_BUS 19
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// DHT configuration
#define DHT11_PIN 23
#define DHT21_PIN 23
#define DHT11_TYPE DHT11
#define DHT21_TYPE DHT22
DHT dht11(DHT11_PIN, DHT11_TYPE);
DHT dht21(DHT21_PIN, DHT21_TYPE);

// Relay pins
#define RELAY1_PIN 33
#define RELAY2_PIN 25
#define RELAY3_PIN 26
#define RELAY4_PIN 27

// Wi-Fi credentials
const char* ssid = "heri.life";
const char* password = "monicacitra";

// Time configuration
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

// Relay control logic
unsigned long previousMillis = 0;
unsigned long interval = 8 * 60 * 60 * 1000;
unsigned long onDuration = 5000;

// Display rotation logic
unsigned long lastDisplayUpdate = 0;
int currentDisplay = 0; // 0: Time, 1: BME280, 2: DHT21, 3: DS18B20

// Web server
AsyncWebServer server(80);

// HTML for web interface
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0; }
    h2 { font-size: 3rem; }
    .relay-controls button { font-size: 1.5rem; margin: 5px; padding: 10px; }
  </style>
</head>
<body>
  <h2>ESP32 Relay Control</h2>
  <div class="relay-controls">
    <button onclick="toggleRelay(1)">Relay 1: %RELAY1STATE%</button><br>
    <button onclick="toggleRelay(2)">Relay 2: %RELAY2STATE%</button><br>
    <button onclick="toggleRelay(3)">Relay 3: %RELAY3STATE%</button><br>
    <button onclick="toggleRelay(4)">Relay 4: %RELAY4STATE%</button><br>
  </div>
  <script>
    function toggleRelay(relayNumber) {
      fetch(`/toggleRelay?relay=${relayNumber}`).then(response => response.text()).then(state => {
        document.querySelector(`button[onclick="toggleRelay(${relayNumber})"]`).innerText = `Relay ${relayNumber}: ${state}`;
      });
    }
  </script>
</body>
</html>
)rawliteral";

// Placeholder replacement for web interface
String processor(const String& var) {
  if (var == "RELAY1STATE") return digitalRead(RELAY1_PIN) ? "ON" : "OFF";
  if (var == "RELAY2STATE") return digitalRead(RELAY2_PIN) ? "ON" : "OFF";
  if (var == "RELAY3STATE") return digitalRead(RELAY3_PIN) ? "ON" : "OFF";
  if (var == "RELAY4STATE") return digitalRead(RELAY4_PIN) ? "ON" : "OFF";
  return String();
}

// Setup function
void setup() {
  Serial.begin(115200);

  // Initialize I2C and sensors
  Wire.begin();
  if (!bme.begin(BME280_I2C_ADDRESS)) {
    Serial.println("BME280 initialization failed!");
    while (1);
  }
  dht11.begin();
  dht21.begin();
  sensors.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_PAGEADDR, 0x3C)) {
    Serial.println("OLED initialization failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.println(WiFi.localIP());

  // Initialize time client
  timeClient.begin();

  // Setup relays
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  // Web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/toggleRelay", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("relay")) {
      int relay = request->getParam("relay")->value().toInt();
      int pin;
      switch (relay) {
        case 1: pin = RELAY1_PIN; break;
        case 2: pin = RELAY2_PIN; break;
        case 3: pin = RELAY3_PIN; break;
        case 4: pin = RELAY4_PIN; break;
        default: request->send(400, "text/plain", "Invalid Relay"); return;
      }
      digitalWrite(pin, !digitalRead(pin));
      request->send(200, "text/plain", digitalRead(pin) ? "ON" : "OFF");
    } else {
      request->send(400, "text/plain", "Missing Relay Parameter");
    }
  });
  server.begin();
}

// Display data on OLED
void updateDisplay() {
  float t, h, p, a;
  float seaLevel = 1013.25; // Standard sea level pressure in hPa
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  switch (currentDisplay) {
    case 0: // BME280 data
      t = bme.readTemperature();
      h = bme.readHumidity();
      p = bme.readPressure()/ 100.0F;
      a = bme.readAltitude(seaLevel);
      Serial.printf("[%s] BME280 Temperature: %.2f°C, Humidity: %.2f%%, Pressure: %.2f hPa, Altitude: %.2fm\n", timeClient.getFormattedTime(), t, h, p, a);
      display.printf("%s\n", timeClient.getFormattedTime());
      display.printf("BME280 Sensor\n");
      display.printf("Temp:%.2fC ", t);
      display.printf("Hum:%.2f%\n", h);
      display.printf("Pres:%.1f ", p);
      display.printf("Alt:%.1fm\n", a);
      break;

    case 1: // DHT21 data
      t = dht21.readTemperature();
      h = dht21.readHumidity();
      Serial.printf("[%s] DHT22 Temperature: %.2f°C, Humidity: %.2f%%\n", timeClient.getFormattedTime(), t, h);
      display.printf("%s\n", timeClient.getFormattedTime());
      display.printf("DHT22 Sensor:\n");
      display.printf("Temp: %.2f C\n", t);
      display.printf("Hum: %.2f %\n", h);
      break;

    case 2: // DS18B20 data
      sensors.requestTemperatures();
      t = sensors.getTempCByIndex(0);
      Serial.printf("[%s] DS18B20 Temperature: %.2f°C\n", timeClient.getFormattedTime(), t);
      display.printf("%s\n", timeClient.getFormattedTime());
      display.printf("DS18B20 Sensor:\n");
      display.printf("Temp: %.2f C\n", t);
      break;
  }

  display.display();
  currentDisplay = (currentDisplay + 1) % 3; // Cycle display mode
}

void loop() {
  timeClient.update();

  // Update display every 5 seconds
  if (millis() - lastDisplayUpdate > 5000) {
    lastDisplayUpdate = millis();
    updateDisplay();
  }

  // Relay control logic
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    digitalWrite(RELAY4_PIN, HIGH);
    delay(onDuration);
    digitalWrite(RELAY4_PIN, LOW);
  }
}
