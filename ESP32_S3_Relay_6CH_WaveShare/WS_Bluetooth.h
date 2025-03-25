#ifndef _WS_BLUETOOTH_H_
#define _WS_BLUETOOTH_H_
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "WS_GPIO.h"
#include "WS_Serial.h"
#include "WS_Config.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"                     // UUID of the server
#define RX_CHARACTERISTIC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"          // UUID of the characteristic Tx
#define TX_CHARACTERISTIC_UUID  "beb5484a-36e1-4688-b7f5-ea07361b26a8"          // UUID of the characteristic Rx

#define Bluetooth_Mode    2


extern void Relay_Analysis(uint8_t *buf,uint8_t Mode_Flag);


void Bluetooth_SendData(char * Data);                                           // Send data using Bluetooth
void Bluetooth_Init();
#endif