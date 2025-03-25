#ifndef WS_SENSORS_H
#define WS_SENSORS_H

#include <Arduino.h>
#define GPIO_PIN_SENSOR_DS18B20 21

void Sensors_Init();          // Initialize sensors
float Read_Temperature();     // Read temperature sensor
float Read_Humidity();        // Read humidity sensor
void Update_Sensor_Readings(); // Update readings and take actions

#endif
