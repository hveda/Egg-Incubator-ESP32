#include "WS_Serial.h"
#include <algorithm>

HardwareSerial lidarSerial(1);  // Using serial port 1
uint8_t data[][8] = {                                       // ESP32-S3-Relay-6CH Control Command (RS485 receiving data)
  { 0x06, 0x05, 0x00, 0x01, 0x55, 0x00, 0xA2, 0xED },       // ESP32-S3-Relay-6CH CH1 Toggle
  { 0x06, 0x05, 0x00, 0x02, 0x55, 0x00, 0x52, 0xED },       // ESP32-S3-Relay-6CH CH2 Toggle
  { 0x06, 0x05, 0x00, 0x03, 0x55, 0x00, 0x03, 0x2D },       // ESP32-S3-Relay-6CH CH3 Toggle
  { 0x06, 0x05, 0x00, 0x04, 0x55, 0x00, 0xB2, 0xEC },       // ESP32-S3-Relay-6CH CH4 Toggle
  { 0x06, 0x05, 0x00, 0x05, 0x55, 0x00, 0xE3, 0x2C },       // ESP32-S3-Relay-6CH CH5 Toggle
  { 0x06, 0x05, 0x00, 0x06, 0x55, 0x00, 0x13, 0x2C },       // ESP32-S3-Relay-6CH CH6 Toggle
  { 0x06, 0x05, 0x00, 0xFF, 0xFF, 0x00, 0xBD, 0xBD },       // ESP32-S3-Relay-6CH ALL ON
  { 0x06, 0x05, 0x00, 0xFF, 0x00, 0x00, 0xFC, 0x4D },       // ESP32-S3-Relay-6CH ALL OFF
};
uint8_t Send_Data[][8] = {                                  // Modbus RTU Relay Control Command (RS485 send data)
  { 0x01, 0x05, 0x00, 0x00, 0x55, 0x00, 0xF2, 0x9A },       // Modbus RTU Relay CH1 Toggle
  { 0x01, 0x05, 0x00, 0x01, 0x55, 0x00, 0xA3, 0x5A },       // Modbus RTU Relay CH2 Toggle
  { 0x01, 0x05, 0x00, 0x02, 0x55, 0x00, 0x53, 0x5A },       // Modbus RTU Relay CH3 Toggle
  { 0x01, 0x05, 0x00, 0x03, 0x55, 0x00, 0x02, 0x9A },       // Modbus RTU Relay CH4 Toggle
  { 0x01, 0x05, 0x00, 0x04, 0x55, 0x00, 0xB3, 0x5B },       // Modbus RTU Relay CH5 Toggle
  { 0x01, 0x05, 0x00, 0x05, 0x55, 0x00, 0xE2, 0x9B },       // Modbus RTU Relay CH6 Toggle
  { 0x01, 0x05, 0x00, 0x06, 0x55, 0x00, 0x12, 0x9B },       // Modbus RTU Relay CH7 Toggle
  { 0x01, 0x05, 0x00, 0x07, 0x55, 0x00, 0x43, 0x5B },       // Modbus RTU Relay CH8 Toggle
  { 0x01, 0x05, 0x00, 0xFF, 0xFF, 0xFF, 0xFC, 0x4A },       // Modbus RTU Relay ALL ON
  { 0x01, 0x05, 0x00, 0xFF, 0x00, 0x00, 0xFD, 0xFA },       // Modbus RTU Relay ALL OFF
};
uint8_t buf[20] = {0};          // Data storage area

void SetData(uint8_t* data, size_t length) {
  lidarSerial.write(data, length);                          // Send data from the RS485
}
void ReadData(uint8_t* buf, uint8_t length) {
  uint8_t Receive_Flag = 0;       
  Receive_Flag = lidarSerial.available();
  if (Receive_Flag >= length) {
    lidarSerial.readBytes(buf, length); 
    char printBuf[length * 3 + 1];
    sprintf(printBuf, "Received data: ");
    for (int i = 0; i < length; i++) {
      sprintf(printBuf + strlen(printBuf), "%02X ", buf[i]); 
    }
    printf(printBuf); 
    /*************************
    Add a receiving data handler
    *************************/
    Receive_Flag = 0;
    memset(buf, 0, sizeof(buf));   
  }
}
void RS485_Analysis(uint8_t *buf)
{
  switch(buf[1])
  {
    case Extension_CH1:
      SetData(Send_Data[0],sizeof(Send_Data[0])); 
      printf("|***  Toggle expansion channel 1 ***|\r\n");
      break;
    case Extension_CH2:
      SetData(Send_Data[1],sizeof(Send_Data[1])); 
      printf("|***  Toggle expansion channel 2 ***|\r\n");
      break;
    case Extension_CH3:
      SetData(Send_Data[2],sizeof(Send_Data[2])); 
      printf("|***  Toggle expansion channel 3 ***|\r\n");
      break;
    case Extension_CH4:
      SetData(Send_Data[3],sizeof(Send_Data[3])); 
      printf("|***  Toggle expansion channel 4 ***|\r\n");
      break;
    case Extension_CH5:
      SetData(Send_Data[4],sizeof(Send_Data[4])); 
      printf("|***  Toggle expansion channel 5 ***|\r\n");
      break;
    case Extension_CH6:
      SetData(Send_Data[5],sizeof(Send_Data[5])); 
      printf("|***  Toggle expansion channel 6 ***|\r\n");
      break;
    case Extension_CH7:
      SetData(Send_Data[6],sizeof(Send_Data[6])); 
      printf("|***  Toggle expansion channel 7 ***|\r\n");
      break;
    case Extension_CH8:
      SetData(Send_Data[7],sizeof(Send_Data[7])); 
      printf("|***  Toggle expansion channel 8 ***|\r\n");
      break;
    case Extension_ALL_ON:
      SetData(Send_Data[8],sizeof(Send_Data[8])); 
      printf("|***  Enable all extension channels ***|\r\n");
      break;
    case Extension_ALL_OFF:
      SetData(Send_Data[9],sizeof(Send_Data[9])); 
      printf("|***  Close all expansion channels ***|\r\n");
      break;
    default:
      printf("Note : Non-control external device instructions !\r\n");
  }
}
void Serial_Init()
{
  Serial.begin(115200);                                  // Initializing serial port
  delay(1000);            // Allow some time for Serial to start
  lidarSerial.begin(115200, SERIAL_8N1, RXD1, TXD1);        // Initializing serial port
}

void Serial_Loop()
{
  uint8_t Receive_Flag = 0;       // Receiving mark
  Receive_Flag = lidarSerial.available();
  if (Receive_Flag > 0) {
    lidarSerial.readBytes(buf, Receive_Flag);              // The Receive_Flag length is read
    if(Receive_Flag == 8){
      uint8_t i=0;
      for(i=0;i<8;i++){
        bool result = std::equal(std::begin(buf), std::begin(buf) + 8, std::begin(data[i]));    // Compare two arrays
        if(result){
          buf[0] = i+1+48;
          Relay_Analysis(buf,RS485_Mode);
          break;
        }
      }   
      if(i > 7)
        printf("Note : Non-instruction data was received - RS485 !\r\n");
    }
    else{
      printf("Note : Non-instruction data was received - RS485 !\r\n");
    }
    Receive_Flag=0;
    memset(buf,0, sizeof(buf));
  }
}