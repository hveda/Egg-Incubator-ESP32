#ifndef _WS_RTC_H_
#define _WS_RTC_H_
#include <Wire.h> 
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "WS_GPIO.h"


extern bool Relay_Flag[6];                    // Relay current status flag

#define RTC_Flag              1               // RTC Timing Switcher flag
#define RTC_OPEN_Time_Hour    16              // Time when the timing switch is enabled
#define RTC_OPEN_Time_Min     29              // Time when the timing switch is enabled
#define RTC_Closs_Time_Hour   16              // Time when the timing switch is off
#define RTC_Closs_Time_Min    30              // Time when the timing switch is off

#define DS3231_I2C_ADDR 0x68
#define I2C_SDA 4
#define I2C_SCL 5
#define GPIO_PIN_SENSOR 3

#define RTC_OPEN_Hour    (RTC_OPEN_Time_Hour%10   + RTC_OPEN_Time_Hour  /10*16)               // RTC Timing Switcher flag
#define RTC_OPEN_Min     (RTC_OPEN_Time_Min%10    + RTC_OPEN_Time_Min   /10*16)               // RTC Timing Switcher flag
#define RTC_Closs_Hour   (RTC_Closs_Time_Hour%10  + RTC_Closs_Time_Hour /10*16)               // RTC Timing Switcher flag
#define RTC_Closs_Min    (RTC_Closs_Time_Min%10   + RTC_Closs_Time_Min  /10*16)               // RTC Timing Switcher flag

void RTC_Init();
void DS3231_ReadTime() ;        
void DS3231_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfWeek, uint8_t day, uint8_t month, uint8_t year);
uint8_t DecToBcd(uint8_t val);
void Acquisition_time();        // Get the network time and set it to DS3231 to be called after the WIFI connection is successful
void RTC_Loop();                // Check whether the relay start-stop time has been reached
#endif