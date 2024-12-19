#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4

// Sensor pin definitions for ESP32-S3-WROOM-1U
#define DHTPIN 5      // Pin for DHT11 (GPIO5)
#define DHTPIN0 4      // Pin for DHT22 (GPIO4)

#define DHTTYPE DHT11    // DHT11 sensor type
#define DHTTYPE0 DHT22    // DHT11 sensor type

// Declare sensor objects
extern DHT dht;
extern DHT dht0;

// Function prototypes
void initSensors();
void readSensors();

#endif
