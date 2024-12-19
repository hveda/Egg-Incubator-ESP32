#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// OLED configuration
#define SCREEN_WIDTH 128 // OLED width in pixels
#define SCREEN_HEIGHT 32 // OLED height in pixels
#define OLED_RESET    -1 // Reset pin (not used with I2C)

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Create BME280 object
Adafruit_BME280 bme;

// I2C address of the BME280 (default is 0x76; some modules use 0x77)
#define BME280_I2C_ADDRESS 0x76
// DHT Sensor pins
#define DHT11_PIN 19  // GPIO for DHT11
#define DHT21_PIN 21  // GPIO for DHT21

// Relay pins
#define RELAY1_PIN 33 // Heater control
#define RELAY2_PIN 25 // Average temperature control
#define RELAY3_PIN 26 // Humidity control
#define RELAY4_PIN 27 // Timed control

// Replace with your network credentials
const char* ssid = "heri.life";
const char* password = "monicacitra";

// DHT sensor types
#define DHT11_TYPE DHT11
#define DHT21_TYPE DHT21

// Temperature and humidity thresholds
float lowerTempThreshold = 30.0;
float upperTempThreshold = 31.5;
float avgTempLowerThreshold = 37.0;
float avgTempUpperThreshold = 38.0;
float humidityLowerThreshold = 50.0;
float humidityUpperThreshold = 60.0;

// Timed relay control
unsigned long previousMillis = 0;
unsigned long interval = 8 * 60 * 60 * 1000; // 3 times a day (8 hours in milliseconds)
unsigned long onDuration = 5000; // Relay4 ON for 5 seconds

// NTP configuration
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // UTC+7, update every 60 seconds (60000 ms)

DHT dht11(DHT11_PIN, DHT11_TYPE);
DHT dht21(DHT21_PIN, DHT21_TYPE);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Web template for displaying data and relay control
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" crossorigin="anonymous">
  <style>
    html { font-family: Arial; text-align: center; margin: 0 auto; }
    h2 { font-size: 3.0rem; }
    .relay-controls { margin-top: 20px; }
    button { font-size: 1.5rem; padding: 10px; margin: 5px; }
  </style>
</head>
<body>
  <h2>ESP32 Relay Control</h2>
  
  <div class="relay-controls">
    <button relay="1" onclick="toggleRelay(1)">Relay 1: %RELAY1STATE%</button><br>
    <button relay="2" onclick="toggleRelay(2)">Relay 2: %RELAY2STATE%</button><br>
    <button relay="3" onclick="toggleRelay(3)">Relay 3: %RELAY3STATE%</button><br>
    <button relay="4" onclick="toggleRelay(4)">Relay 4: %RELAY4STATE%</button><br>
  </div>

  <script>
    // Function to toggle a relay and update the button text
    function toggleRelay(relayNumber) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
          // Find the button for the relay and update its text
          const button = document.querySelector(`button[relay="${relayNumber}"]`);
          if (button) {
            button.innerText = `Relay ${relayNumber}: ${this.responseText}`;
          } else {
            console.error("Button not found for relay", relayNumber);
          }
        }
      };
      // Send the request to the server
      xhttp.open("GET", `/toggleRelay?relay=${relayNumber}`, true);
      xhttp.send();
    }
  </script>
</body>
</html>
)rawliteral";

// Replace placeholders with actual data
String processor(const String& var){
  if(var == "DHT11_TEMPERATURE"){
    return String(dht11.readTemperature());
  }
  if(var == "DHT11_HUMIDITY"){
    return String(dht11.readHumidity());
  }
  if(var == "DHT21_TEMPERATURE"){
    return String(dht21.readTemperature());
  }
  if(var == "DHT21_HUMIDITY"){
    return String(dht21.readHumidity());
  }
  if(var == "TIME"){
    timeClient.update();
    return timeClient.getFormattedTime();
  }
  if(var == "LOWERTEMPTHRESHOLD"){
    return String(lowerTempThreshold);
  }
  if(var == "UPPERTEMPTHRESHOLD"){
    return String(upperTempThreshold);
  }
  if(var == "AVGTEMPLOWERTHRESHOLD"){
    return String(avgTempLowerThreshold);
  }
  if(var == "AVGTEMPERATUREUPPERTHRESHOLD"){
    return String(avgTempUpperThreshold);
  }
  if(var == "HUMIDITYLOWERTHRESHOLD"){
    return String(humidityLowerThreshold);
  }
  if(var == "HUMIDITYUPPERTHRESHOLD"){
    return String(humidityUpperThreshold);
  }
  if(var == "RELAY1STATE"){
    return digitalRead(RELAY1_PIN) ? "ON" : "OFF";
  }
  if(var == "RELAY2STATE"){
    return digitalRead(RELAY2_PIN) ? "ON" : "OFF";
  }
  if(var == "RELAY3STATE"){
    return digitalRead(RELAY3_PIN) ? "ON" : "OFF";
  }
  if(var == "RELAY4STATE"){
    return digitalRead(RELAY4_PIN) ? "ON" : "OFF";
  }
  return String();
}

void displayBME280Data() {
  // Read BME280 values
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Convert to hPa

  // Print values to Serial Monitor
  Serial.println("BME280 Readings:");
  Serial.println("Temperature: " + String(temperature) + " °C");
  Serial.println("Humidity: " + String(humidity) + " %");
  Serial.println("Pressure: " + String(pressure) + " hPa");

  // Optionally display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("BME280 Data:");
  display.println("Temp: " + String(temperature) + " C");
  display.println("Hum: " + String(humidity) + " %");
  display.println("Pres: " + String(pressure) + " hPa");
  display.display();
}

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Initialize I2C bus
  Wire.begin();

  // Initialize BME280
  if (!bme.begin(BME280_I2C_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor!");
    while (1); // Stop if initialization fails
  }
  Serial.println("BME280 Initialized");
  // Initialize OLED display
  if (!display.begin(SSD1306_PAGEADDR, 0x3C)) { // 0x3C is the default I2C address
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);      // Text size
  display.setTextColor(SSD1306_WHITE); // Text color
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F("Initializing..."));
  display.display();           // Display "Initializing..."

  // Initialize DHT sensors
  dht11.begin();
  dht21.begin();

  // Initialize relays
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Initialize NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(7 * 3600); // UTC+7

  // Initialize web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Toggle relay state
  server.on("/toggleRelay", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("relay")) {
      int relayNumber = request->getParam("relay")->value().toInt();
      bool state;
      switch (relayNumber) {
        case 1:
          state = !digitalRead(RELAY1_PIN);
          digitalWrite(RELAY1_PIN, state);
          break;
        case 2:
          state = !digitalRead(RELAY2_PIN);
          digitalWrite(RELAY2_PIN, state);
          break;
        case 3:
          state = !digitalRead(RELAY3_PIN);
          digitalWrite(RELAY3_PIN, state);
          break;
        case 4:
          state = !digitalRead(RELAY4_PIN);
          digitalWrite(RELAY4_PIN, state);
          break;
        default:
          request->send(400, "text/plain", "Invalid Relay Number");
          return;
      }
      request->send(200, "text/plain", state ? "ON" : "OFF");
    } else {
      request->send(400, "text/plain", "Missing Relay Parameter");
    }
  });

  // Serve individual sensor data
  server.on("/dht11_temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    float temperature = dht11.readTemperature();
    request->send(200, "text/plain", String(temperature));
  });

  server.on("/dht11_humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    float humidity = dht11.readHumidity();
    request->send(200, "text/plain", String(humidity));
  });

  server.on("/dht21_temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    float temperature = dht21.readTemperature();
    request->send(200, "text/plain", String(temperature));
  });

  server.on("/dht21_humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    float humidity = dht21.readHumidity();
    request->send(200, "text/plain", String(humidity));
  });

  // Serve current time
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    String timeString = timeClient.getFormattedTime();
    request->send(200, "text/plain", timeString);
  });

  // Handle form submission
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("lowerTempThreshold")) {
      lowerTempThreshold = request->getParam("lowerTempThreshold")->value().toFloat();
    }
    if (request->hasParam("upperTempThreshold")) {
      upperTempThreshold = request->getParam("upperTempThreshold")->value().toFloat();
    }
    if (request->hasParam("avgTempLowerThreshold")) {
      avgTempLowerThreshold = request->getParam("avgTempLowerThreshold")->value().toFloat();
    }
    if (request->hasParam("avgTempUpperThreshold")) {
      avgTempUpperThreshold = request->getParam("avgTempUpperThreshold")->value().toFloat();
    }
    if (request->hasParam("humidityLowerThreshold")) {
      humidityLowerThreshold = request->getParam("humidityLowerThreshold")->value().toFloat();
    }
    if (request->hasParam("humidityUpperThreshold")) {
      humidityUpperThreshold = request->getParam("humidityUpperThreshold")->value().toFloat();
    }
    request->redirect("/");
  });

  // Start the server
  server.begin();
}

void loop() {
  // Update time client
  timeClient.update();

  // Relay control logic based on temperature and humidity thresholds
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(RELAY4_PIN, HIGH);
    delay(onDuration);
    digitalWrite(RELAY4_PIN, LOW);
  }
  displayBME280Data();
  delay(2000);
}
