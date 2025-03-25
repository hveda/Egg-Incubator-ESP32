#ifndef _WS_WIFI_H_
#define _WS_WIFI_H_

#include <WiFi.h>
#include <WebServer.h> 
#include "WS_GPIO.h"


#define WIFI_Mode            3


extern bool Relay_Flag[6];       // Relay current status flag
extern void Relay_Analysis(uint8_t *buf,uint8_t Mode_Flag);

void handleRoot();
void handleGetData();
void handleSwitch(int ledNumber);

void handleSwitch1();
void handleSwitch2();
void handleSwitch3();
void handleSwitch4();
void handleSwitch5();
void handleSwitch6();
void handleSwitch7();
void handleSwitch8();
void WIFI_Init();
void WIFI_Loop();

#endif