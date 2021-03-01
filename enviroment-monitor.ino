#include "config.h"
#include "EEPROM.h"
#include "ETH.h"
#include "NTP.h"
#include "BME280.h"
#include "MQTT.h"
#include "WebServer.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  xTaskCreatePinnedToCore(
    Task1code,
    "Task1",
    10000,
    NULL,
    1,
    &Task1,
    0);
  delay(500);

  xTaskCreatePinnedToCore(
    Task2code,
    "Task2",
    10000,
    NULL,
    1,
    &Task2,
    1);
  delay(500);
}

void Task1code( void * pvParameters ) {
  Serial.begin(115200);

  EEPROM.begin(512);

  if (!isCharValid(readEEPROM(AdminUsernameStart, AdminUsernameLength)))
    writeEEPROM(AdminUsernameStart, AdminUsernameLength, "admin");

  if (!isCharValid(readEEPROM(AdminPasswordStart, AdminPasswordLength)))
    writeEEPROM(AdminPasswordStart, AdminPasswordLength, "admin");

  WiFi.onEvent(WiFiEvent);
  ETH.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  bmeStatus = initBME();
  initializeServer();

  Serial.println(ETH.macAddress());

  for (;;) {
    delay(10);
  }
}

void Task2code( void * pvParameters ) {

  client.setCallback(callback);

  for (;;) {
    delay(10);
    if (eth_connected) {
      if (!client.connected()) {
        reconnect();
      }

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        DataPublish();
      }

      client.loop();
    }
  }
}

void loop() {}
