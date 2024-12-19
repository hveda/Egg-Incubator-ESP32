#include "SensorHandler.h"
#include "WS_WIFI.h"
#include "WS_GPIO.h"
#include "WS_RTC.h"
#include <Arduino.h>

// Initialize the sensors
DHT dht(DHTPIN, DHTTYPE);
DHT dht0(DHTPIN0, DHTTYPE0);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
int heaterRelayPin = GPIO_PIN_CH1; // Example GPIO pin
int fanRelayPin = GPIO_PIN_CH2;    // Example GPIO pin
float lowerTemp = 37.3;
float upperTemp = 38.0;
float targetTemp = 37.5;

// Function to initialize sensors
void initSensors() {
    dht.begin();
    // dht0.begin();
    sensors.begin();
}

void controlRelays(float currentTemp1) {
  // #define GPIO_PIN_CH1      1     // CH1 Control GPIO
  // #define GPIO_PIN_CH2      2     // Ch Control GPIO
  // #define GPIO_PIN_CH3      41    // CH3 Control GPIO
  // #define GPIO_PIN_CH4      42    // CH4 Control GPIO
  // #define GPIO_PIN_CH5      45    // CH5 Control GPIO
  // #define GPIO_PIN_CH6      46    // CH6 Control GPIO
  // Determine the target relay state
  if (currentTemp1 < targetTemp && !Relay_Flag[0]) {
    digitalWrite(GPIO_PIN_CH1, HIGH);
    Relay_Flag[0] = true; // Update the state
    printf("|***  Relay 1 on ***|\r\n");
  } else if (currentTemp1 > targetTemp + 0.2 && Relay_Flag[0]) {
    digitalWrite(GPIO_PIN_CH1, LOW);
    Relay_Flag[0] = false; // Update the state
    printf("|***  Relay 1 off ***|\r\n");
  } else {
    printf("|*** No relay state changes ***| Current State: %s\r\n", 
               Relay_Flag[0] ? "ON" : "OFF");
  }
}


// Function to read sensor data
void readSensors() {
    // Read data from DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Get current time
    String currentTime = timeClient.getFormattedTime();

    // Print data in a single line
    if (isnan(h) || isnan(t)) {
        printf("[%s] DHT11 Failed to read from sensors!\n", currentTime.c_str());
    } else {
        printf("[%s] DHT11 Temperature: %.2f°C, Humidity: %.2f%%\n", currentTime.c_str(), t, h);
        // controlRelays(t);
    }

    // Read data from DHT22/AM2301
    // float h0 = dht0.readHumidity();
    // float t0 = dht0.readTemperature();

    // // Print data in a single line
    // if (isnan(h0) || isnan(t0)) {
    //     printf("[%s] DHT22 Failed to read from sensors!\n", currentTime.c_str());
    // } else {
    //     printf("[%s] DHT22 Temperature: %.2f°C, Humidity: %.2f%%\n", currentTime.c_str(), t0, h0);
    // }
    sensors.requestTemperatures(); // Send the command to get temperatures
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    float tempC = sensors.getTempCByIndex(0);

    // Check if reading was successful
    if(tempC != DEVICE_DISCONNECTED_C) 
    {
      printf("[%s] Temperature for the device 1 (index 0) is: %.2f°C\n",currentTime.c_str(), tempC);
      controlRelays(tempC);
    } 
    else
    {
      printf("Error: Could not read temperature data\n");
    }
}


