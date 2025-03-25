#include "WS_Sensors.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_AHTX0.h>

#define ONE_WIRE_BUS GPIO_PIN_SENSOR_DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

Adafruit_AHTX0 aht10;

void Sensors_Init() {
  ds18b20.begin();
  if (!aht10.begin()) {
    Serial.println("Failed to initialize AHT10 sensor");
  }
}

float Read_DS18B20_Temperature() {
  ds18b20.requestTemperatures();
  return ds18b20.getTempCByIndex(0);  // Read temperature from the first sensor
}

float Read_AHT10_Temperature() {
  sensors_event_t temp, hum;
  aht10.getEvent(&hum, &temp);
  return temp.temperature;
}

float Read_AHT10_Humidity() {
  sensors_event_t temp, hum;
  aht10.getEvent(&hum, &temp);
  return hum.relative_humidity;
}
