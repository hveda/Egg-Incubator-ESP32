#include "WS_RTC.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN GPIO_PIN_SENSOR
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1  // Reset pin (not used for I2C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

uint8_t Time[8] = {0};
char *week[] = {"SUN", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};

bool RTC_Open_OK = 1;
bool RTC_Closs_OK = 1;

// Initialize the RTC, Sensors and OLED
void RTC_Init() {
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  delay(1000);

  if (!display.begin(SSD1306_PAGEADDR, OLED_RESET)) {
    printf("OLED initialization failed\r\n");
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RTC Initializing...");
  display.display();
  delay(1000);
}

// Read time from the DS3231
void DS3231_ReadTime() {   
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
    
    Time[0] = Time[0] & 0x7F;
    Time[1] = Time[1] & 0x7F;
    Time[2] = Time[2] & 0x3F;
    Time[3] = Time[3] & 0x07;
    Time[4] = Time[4] & 0x3F;
    Time[5] = Time[5] & 0x1F;
  }
}

// Convert decimal to BCD
uint8_t DecToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

// Set time to the DS3231
void DS3231_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year) {
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(0x00); // Start at the seconds register
  Wire.write(DecToBcd(sec));
  Wire.write(DecToBcd(min));
  Wire.write(DecToBcd(hour));
  Wire.write(DecToBcd(dayOfWeek));
  Wire.write(DecToBcd(dayOfMonth));
  Wire.write(DecToBcd(month));
  Wire.write(DecToBcd(year));
  Wire.endTransmission();
}

// Get network time and set RTC
void Acquisition_time() {                                   
  timeClient.begin();
  timeClient.setTimeOffset(8 * 3600);                       
  timeClient.update();

  time_t currentTime = timeClient.getEpochTime();
  while (currentTime < 1609459200) {
    timeClient.update();  
    currentTime = timeClient.getEpochTime();
  }

  struct tm *localTime = localtime(&currentTime);
  if (localTime->tm_year < (2021 - 1900)) {
      // Fallback to NTP time if RTC time is not valid
      timeClient.update();
      currentTime = timeClient.getEpochTime();
      localTime = localtime(&currentTime);
  }

  DS3231_SetTime(localTime->tm_sec, localTime->tm_min, localTime->tm_hour, 
                localTime->tm_wday, localTime->tm_mday, localTime->tm_mon + 1, 
                localTime->tm_year - 100);
}

// Display time and date on the OLED
void displayTimeOnOLED() {
  char buffer[20];

  DS3231_ReadTime();
  display.clearDisplay();
  display.setCursor(0, 0);

  // Format time
  snprintf(buffer, sizeof(buffer), "Time: %02X:%02X:%02X", Time[2], Time[1], Time[0]);
  display.println(buffer);

  // Format date
  snprintf(buffer, sizeof(buffer), "Date: %02X/%02X/20%02X", Time[4], Time[5], Time[6]);
  display.println(buffer);

  // Display weekday
  snprintf(buffer, sizeof(buffer), "Day: %s", week[Time[3] - 1]);
  display.println(buffer);

  display.display();
}

// Main loop for RTC and OLED
void RTC_Loop() {
  DS3231_ReadTime();
  displayTimeOnOLED();

  if (Time[2] == RTC_OPEN_Hour && Time[1] == RTC_OPEN_Min && RTC_Flag == 1 && RTC_Open_OK == 1) {
    RTC_Open_OK = 0;
    digitalWrite(GPIO_PIN_CH1, HIGH);
    digitalWrite(GPIO_PIN_CH2, HIGH);
    digitalWrite(GPIO_PIN_CH3, HIGH);
    digitalWrite(GPIO_PIN_CH4, HIGH);
    digitalWrite(GPIO_PIN_CH5, HIGH);
    digitalWrite(GPIO_PIN_CH6, HIGH);
    memset(Relay_Flag, 1, sizeof(Relay_Flag));
    Buzzer_PWM(300);
    printf("|***  Relay ALL on  ***|\r\n");
  } else if (Time[2] == RTC_Closs_Hour && Time[1] == RTC_Closs_Min && RTC_Flag == 1 && RTC_Closs_OK == 1) {
    RTC_Closs_OK = 0;
    digitalWrite(GPIO_PIN_CH1, LOW);
    digitalWrite(GPIO_PIN_CH2, LOW);
    digitalWrite(GPIO_PIN_CH3, LOW);
    digitalWrite(GPIO_PIN_CH4, LOW);
    digitalWrite(GPIO_PIN_CH5, LOW);
    digitalWrite(GPIO_PIN_CH6, LOW);
    memset(Relay_Flag, 0, sizeof(Relay_Flag));
    Buzzer_PWM(100);
    delay(100);
    Buzzer_PWM(100);
    printf("|***  Relay ALL off ***|\r\n");
  }

  // Add sensor reading
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (!isnan(temp) && !isnan(hum)) {
    Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temp, hum);
    // Example action: Adjust relay based on temperature
    if (temp > 37.5) digitalWrite(GPIO_PIN_CH1, HIGH);  // Turn on cooling
    if (temp < 36.5) digitalWrite(GPIO_PIN_CH1, LOW);   // Turn off cooling
  }

  if (RTC_Flag == 1 && Time[1] != RTC_OPEN_Min) {
    RTC_Open_OK = 1;
  }

  if (RTC_Flag == 1 && Time[1] != RTC_Closs_Min) {
    RTC_Closs_OK = 1;
  }
}
