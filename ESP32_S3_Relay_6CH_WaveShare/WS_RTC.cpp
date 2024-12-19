#include "WS_RTC.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

uint8_t Time[8] = {0};
char *week[] = {"SUN","Mon","Tues","Wed","Thur","Fri","Sat"};

bool RTC_Open_OK = 1;              // RTC Timing Switcher flag
bool RTC_Closs_OK = 1;              // RTC Timing Switcher flag

void RTC_Init() {
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);
}

void DS3231_ReadTime() 
{   
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(0x00); // Start from seconds register
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDR, 7);
  
  if (Wire.available() >= 7) {
    Time[0] = Wire.read(); // Seconds
    Time[1] = Wire.read(); // Minutes
    Time[2] = Wire.read(); // Hours
    Time[3] = Wire.read(); // Weekday
    Time[4] = Wire.read(); // Day
    Time[5] = Wire.read(); // Month
    Time[6] = Wire.read(); // Year
    
  
    Time[0] = Time[0]&0x7F;  // Seconds
    Time[1] = Time[1]&0x7F;  // Minutes
    Time[2] = Time[2]&0x3F;  // Hours
    Time[3] = Time[3]&0x07;  // Weekday
    Time[4] = Time[4]&0x3F;  // Day
    Time[5] = Time[5]&0x1F;  // Month
    Time[6] = Time[6];       // Year
    Time[7] = 0x20;          // Year
  }
}
void DS3231_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfWeek, uint8_t day, uint8_t month, uint8_t year) {
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(0); // 秒寄存器
  Wire.write(DecToBcd(sec));
  Wire.write(DecToBcd(min));
  Wire.write(DecToBcd(hour));
  Wire.write(DecToBcd(dayOfWeek));
  Wire.write(DecToBcd(day));
  Wire.write(DecToBcd(month));
  Wire.write(DecToBcd(year));
  Wire.endTransmission();
  printf("Completion time update\r\n");
  
}

uint8_t DecToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

void Acquisition_time() {                                   // Get the network time and set to DS3231 to be called after the WIFI connection is successful
  timeClient.begin();
  timeClient.setTimeOffset(8 * 3600);                       // Set the time zone, here use East 8 (Beijing time)
  timeClient.update();

  time_t currentTime = timeClient.getEpochTime();
  while(currentTime < 1609459200)                           // Using the current timestamp to compare with a known larger value,1609459200 is a known larger timestamp value that corresponds to January 1, 2021
  {
    timeClient.update();  
    currentTime = timeClient.getEpochTime();
  }
  struct tm *localTime = localtime(&currentTime);
  DS3231_SetTime(localTime->tm_sec, localTime->tm_min, localTime->tm_hour, localTime->tm_wday, localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year - 100);  
  
  // Turn off the WiFi connection
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void RTC_Loop()
{
  DS3231_ReadTime();
  if(Time[2] == RTC_OPEN_Hour && Time[1] == RTC_OPEN_Min && RTC_Flag == 1 && RTC_Open_OK == 1){           // Turn on all relays on time

    RTC_Open_OK =0;
    digitalWrite(GPIO_PIN_CH1, HIGH);                                       // Open CH1 relay
    digitalWrite(GPIO_PIN_CH2, HIGH);                                       // Open CH2 relay
    digitalWrite(GPIO_PIN_CH3, HIGH);                                       // Open CH3 relay
    digitalWrite(GPIO_PIN_CH4, HIGH);                                       // Open CH4 relay
    digitalWrite(GPIO_PIN_CH5, HIGH);                                       // Open CH5 relay
    digitalWrite(GPIO_PIN_CH6, HIGH);                                       // Open CH6 relay
    memset(Relay_Flag,1, sizeof(Relay_Flag));
    Buzzer_PWM(300);
    printf("|***  Relay ALL on  ***|\r\n");
  }
  else if(Time[2] == RTC_Closs_Hour && Time[1] == RTC_Closs_Min && RTC_Flag == 1 && RTC_Closs_OK == 1){   // Turn off all relays regularly
    RTC_Closs_OK = 0;
    digitalWrite(GPIO_PIN_CH1, LOW);                                        // Turn off CH1 relay
    digitalWrite(GPIO_PIN_CH2, LOW);                                        // Turn off CH2 relay
    digitalWrite(GPIO_PIN_CH3, LOW);                                        // Turn off CH3 relay
    digitalWrite(GPIO_PIN_CH4, LOW);                                        // Turn off CH4 relay
    digitalWrite(GPIO_PIN_CH5, LOW);                                        // Turn off CH5 relay
    digitalWrite(GPIO_PIN_CH6, LOW);                                        // Turn off CH6 relay
    memset(Relay_Flag,0, sizeof(Relay_Flag));
    Buzzer_PWM(100);
    delay(100);
    Buzzer_PWM(100);
    printf("|***  Relay ALL off ***|\r\n");
  }
  if(RTC_Flag == 1 && Time[1] != RTC_OPEN_Min){
    RTC_Open_OK = 1;
  
  }if(RTC_Flag == 1 && Time[1] != RTC_Closs_Min){
    RTC_Closs_OK = 1;
  }  
}