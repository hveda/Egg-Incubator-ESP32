#ifndef _WS_GPIO_H_
#define _WS_GPIO_H_
#include <HardwareSerial.h>     // Reference the ESP32 built-in serial port library

/*************************************************************  I/O  *************************************************************/
#define TXD1 17                 //The TXD of UART1 corresponds to GPIO
#define RXD1 18                 //The RXD of UART1 corresponds to GPIO
#define GPIO_PIN_CH1      1     // CH1 Control GPIO
#define GPIO_PIN_CH2      2     // CH2 Control GPIO
#define GPIO_PIN_CH3      41    // CH3 Control GPIO
#define GPIO_PIN_CH4      42    // CH4 Control GPIO
#define GPIO_PIN_CH5      45    // CH5 Control GPIO
#define GPIO_PIN_CH6      46    // CH6 Control GPIO
#define GPIO_PIN_RGB      38    // RGB Control GPIO
#define GPIO_PIN_Buzzer   21    // Buzzer Control GPIO

/***********************************************************  Buzzer  ***********************************************************/
#define PWM_Channel     1       // PWM Channel   
#define Frequency       1000     // PWM frequencyconst    
#define Resolution      8       
#define Dutyfactor      200     

/*************************************************************  I/O  *************************************************************/
void digitalToggle(int pin);
void RGB_Light(uint8_t red_val, uint8_t green_val, uint8_t blue_val);
void GPIO_Init();
void Buzzer_PWM(uint16_t Time);

#endif