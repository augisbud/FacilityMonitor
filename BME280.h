#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

float lastTemperature, lastHumidity = 0;
bool bmeStatus = false;

bool initBME() {
  Wire.begin();
  bool status = bme.begin(0x76);

  return status;
}

float getBME(String type) {
  if (type == "temperature")
    return bmeStatus ? bme.readTemperature() : 255.0;
  else if (type == "humidity")
    return bmeStatus ? bme.readHumidity() : 255.0;
  else if (type == "pressure")
    return bmeStatus ? bme.readPressure() / 100.0F : 255.0;
  else
    return NULL;
}
