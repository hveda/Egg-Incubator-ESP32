#include "WS_WIFI.h"
#include "WS_Bluetooth.h"
#include "WS_GPIO.h"
#include "WS_Serial.h"
#include "WS_RTC.h"
#include "SensorHandler.h"


#define CH1 '1'                 // CH1 Enabled Instruction
#define CH2 '2'                 // CH2 Enabled Instruction
#define CH3 '3'                 // CH3 Enabled Instruction
#define CH4 '4'                 // CH4 Enabled Instruction
#define CH5 '5'                 // CH5 Enabled Instruction
#define CH6 '6'                 // CH6 Enabled Instruction
#define ALL_ON  '7'             // Start all channel instructions
#define ALL_OFF '8'             // Disable all channel instructions

#define RS485_Mode        1     // Used to distinguish data sources
#define Bluetooth_Mode    2
#define WIFI_Mode         3

extern char ipStr[16];
extern bool WIFI_Connection;

bool Relay_Flag[6] = {0};       // Relay current status flag
uint32_t Simulated_time=0;      // Analog time counting

       
/********************************************************  Data Analysis  ********************************************************/
void Relay_Analysis(uint8_t *buf,uint8_t Mode_Flag)
{
  if(Mode_Flag == Bluetooth_Mode)
    printf("Bluetooth Data :");
  else if(Mode_Flag == WIFI_Mode)
    printf("WIFI Data :");
  else
    printf("RS485 Data :");  
  switch(buf[0])
  {
    case CH1: 
      digitalToggle(GPIO_PIN_CH1);                                              //Toggle the level status of the GPIO_PIN_CH1 pin
      Relay_Flag[0] =! Relay_Flag[0];
      Buzzer_PWM(100);
      if(Relay_Flag[0])
        printf("|***  Relay CH1 on  ***|\r\n");
      else
        printf("|***  Relay CH1 off ***|\r\n");
      break;
    case CH2: 
      digitalToggle(GPIO_PIN_CH2);                                             //Toggle the level status of the GPIO_PIN_CH2 pin
      Relay_Flag[1] =! Relay_Flag[1];
      Buzzer_PWM(100);
      if(Relay_Flag[1])
        printf("|***  Relay CH2 on  ***|\r\n");
      else
        printf("|***  Relay CH2 off ***|\r\n");
      break;
    case CH3:
      digitalToggle(GPIO_PIN_CH3);                                             //Toggle the level status of the GPIO_PIN_CH3 pin
      Relay_Flag[2] =! Relay_Flag[2];
      Buzzer_PWM(100);
      if(Relay_Flag[2])
        printf("|***  Relay CH3 on  ***|\r\n");
      else
        printf("|***  Relay CH3 off ***|\r\n");
      break;
    case CH4:
      digitalToggle(GPIO_PIN_CH4);                                             //Toggle the level status of the GPIO_PIN_CH4 pin
      Relay_Flag[3] =! Relay_Flag[3];
      Buzzer_PWM(100);
      if(Relay_Flag[3])
        printf("|***  Relay CH4 on  ***|\r\n");
      else
        printf("|***  Relay CH4 off ***|\r\n");
      break;
    case CH5:
      digitalToggle(GPIO_PIN_CH5);                                             //Toggle the level status of the GPIO_PIN_CH5 pin
      Relay_Flag[4] =! Relay_Flag[4];
      Buzzer_PWM(100);
      if(Relay_Flag[4])
        printf("|***  Relay CH5 on  ***|\r\n");
      else
        printf("|***  Relay CH5 off ***|\r\n");
      break;
    case CH6:
      digitalToggle(GPIO_PIN_CH6);                                             //Toggle the level status of the GPIO_PIN_CH6 pin
      Relay_Flag[5] =! Relay_Flag[5];
      Buzzer_PWM(100);
      if(Relay_Flag[5])
        printf("|***  Relay CH6 on  ***|\r\n");
      else
        printf("|***  Relay CH6 off ***|\r\n");
      break;
    case ALL_ON:
      digitalWrite(GPIO_PIN_CH1, HIGH);                                       // Open CH1 relay
      digitalWrite(GPIO_PIN_CH2, HIGH);                                       // Open CH2 relay
      digitalWrite(GPIO_PIN_CH3, HIGH);                                       // Open CH3 relay
      digitalWrite(GPIO_PIN_CH4, HIGH);                                       // Open CH4 relay
      digitalWrite(GPIO_PIN_CH5, HIGH);                                       // Open CH5 relay
      digitalWrite(GPIO_PIN_CH6, HIGH);                                       // Open CH6 relay
      memset(Relay_Flag,1, sizeof(Relay_Flag));
      printf("|***  Relay ALL on  ***|\r\n");
      Buzzer_PWM(300);
      break;
    case ALL_OFF:
      digitalWrite(GPIO_PIN_CH1, LOW);                                        // Turn off CH1 relay
      digitalWrite(GPIO_PIN_CH2, LOW);                                        // Turn off CH2 relay
      digitalWrite(GPIO_PIN_CH3, LOW);                                        // Turn off CH3 relay
      digitalWrite(GPIO_PIN_CH4, LOW);                                        // Turn off CH4 relay
      digitalWrite(GPIO_PIN_CH5, LOW);                                        // Turn off CH5 relay
      digitalWrite(GPIO_PIN_CH6, LOW);                                        // Turn off CH6 relay
      memset(Relay_Flag,0, sizeof(Relay_Flag));
      printf("|***  Relay ALL off ***|\r\n");
      Buzzer_PWM(100);
      delay(100);
      Buzzer_PWM(100);
      break;
    default:
      printf("Note : Non-instruction data was received !\r\n");
  }
}

/********************************************************  Initializing  ********************************************************/
void setup() {
// UART
  Serial_Init();   

// Relay . RGB . Buzzer GPIO
  GPIO_Init();      

// RTC
  if(RTC_Enable)
  {
    RTC_Init();
  }             

// Bluetooth
  Bluetooth_Init();  

// WIFI
  if(WIFI_Enable == 1)      // WIFI connection successful
    WIFI_Init();            

// Obtain and synchronize network time
  if(WIFI_Connection == 1 && RTC_Enable == 1){
    Acquisition_time();
  }
}

/**********************************************************  While  **********************************************************/
void loop() {
// RS485 Receive Data
  Serial_Loop();
  
// Bluetooth Receive Data
  // The operation after receiving the data is processed in Bluetooth.C

// WIFI
if(WIFI_Enable == 1)
  WIFI_Loop();

  Simulated_time++;
// Send WIFI IP via Bluetooth
  if(WIFI_Connection == 1){
    if(Simulated_time % 1 == 0){
      Bluetooth_SendData(ipStr);        // The IP address that sends WIFI
    }
  }
// RTC
  if(RTC_Enable)
  {
    if(Simulated_time % 1 == 0){
      RTC_Loop();
    }
  }
  if(Simulated_time % 5 ==0){
    // Read sensor data
    readSensors();
  }
  if(Simulated_time == 5)
    Simulated_time = 0;
  delay(1000);
}