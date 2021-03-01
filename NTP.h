#include <Time.h>

const char* ntpServer = "ntp2.litnet.lt";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;

String localTime() {
  time_t rawtime;
  
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  
  char timeStringBuff[50];
  
  strftime(timeStringBuff, sizeof(timeStringBuff), "%b %d %Y %H:%M:%S", &timeinfo);
  
  return String(timeStringBuff);
}
