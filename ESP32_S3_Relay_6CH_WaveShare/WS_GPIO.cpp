#include "WS_GPIO.h"

/*************************************************************  I/O  *************************************************************/
void digitalToggle(int pin)
{
  digitalWrite(pin, !digitalRead(pin));                                         // Toggle the state of the pin
}
void RGB_Light(uint8_t red_val, uint8_t green_val, uint8_t blue_val)
{
  neopixelWrite(GPIO_PIN_RGB,green_val,red_val,blue_val);                       // RGB color adjustment
}
void Buzzer_PWM(uint16_t Time)                                                  //ledChannel：PWM Channe    dutyfactor:dutyfactor
{
  ledcWrite(PWM_Channel, Dutyfactor);
  delay(Time);
  ledcWrite(PWM_Channel, 0);
}
void GPIO_Init()
{
  /*************************************************************************
  Relay GPIO
  *************************************************************************/
  pinMode(GPIO_PIN_CH1, OUTPUT);                            // Initialize the control GPIO of relay CH1
  pinMode(GPIO_PIN_CH2, OUTPUT);                            // Initialize the control GPIO of relay CH2
  pinMode(GPIO_PIN_CH3, OUTPUT);                            // Initialize the control GPIO of relay CH3
  pinMode(GPIO_PIN_CH4, OUTPUT);                            // Initialize the control GPIO of relay CH4
  pinMode(GPIO_PIN_CH5, OUTPUT);                            // Initialize the control GPIO of relay CH5
  pinMode(GPIO_PIN_CH6, OUTPUT);                            // Initialize the control GPIO of relay CH6
  pinMode(GPIO_PIN_RGB, OUTPUT);                            // Initialize the control GPIO of RGB
  pinMode(GPIO_PIN_Buzzer, OUTPUT);                         // Initialize the control GPIO of Buzzer
  
  ledcSetup(PWM_Channel, Frequency, Resolution);            // Set PWM channel
  ledcAttachPin(GPIO_PIN_Buzzer, PWM_Channel);              // Connect the channel to the corresponding pin

}