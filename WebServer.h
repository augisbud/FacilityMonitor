#include "ESPAsyncWebServer.h"

#include "FS.h"
#include <LITTLEFS.h>

AsyncWebServer server(80);

String updateStatus = "";
String updateURL = "";

TaskHandle_t Task3;

void updateTask( void * pvParameters ) {
  t_httpUpdate_return ret = ESPhttpUpdate.update(updateURL);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      updateStatus = "HTTP_UPDATE_FAILED Error (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString() ;
      break;

    case HTTP_UPDATE_NO_UPDATES:
      updateStatus = "HTTP_UPDATE_NO_UPDATES";
      break;

    case HTTP_UPDATE_OK:
      updateStatus = "HTTP_UPDATE_OK";
      break;
  }
  vTaskDelete(NULL);
}

String processor(const String& var) {
  if (var == "MacAddress")
    return ETH.macAddress();
  else if (var == "Version")
    return VERSION;
  else if (var == "MQTTStatus")
    return client.connected() ? "Connected" : "Disconnected";
  else if (var == "MQTTTOPIC")
    return readEEPROM(MQTTTopicStart, MQTTTopicLength);
  else if (var == "MQTTIP")
    return readEEPROM(MQTTIPStart, MQTTIPLength);
  else if (var == "MQTTPORT")
    return readEEPROM(MQTTPortStart, MQTTPortLength);
  else if (var == "MQTTUSERNAME")
    return readEEPROM(MQTTUsernameStart, MQTTUsernameLength);
  else if (var == "MQTTPASSWORD")
    return readEEPROM(MQTTPasswordStart, MQTTPasswordLength);
  else if (var == "USERNAME")
    return readEEPROM(AdminUsernameStart, AdminUsernameLength);
  else if (var == "PASSWORD")
    return readEEPROM(AdminPasswordStart, AdminPasswordLength);
  else if (var == "Temperature")
    return String(getBME("temperature"));
  else if (var == "Humidity")
    return String(getBME("humidity"));
  else if (var == "Pressure")
    return String(getBME("pressure"));
  else if (var == "IPv4")
    return ETH.localIP().toString();
  else if (var == "GATEWAY")
    return ETH.gatewayIP().toString();
  else if (var == "SUBNETMASK")
    return ETH.subnetMask().toString();
  else if (var == "LINKSPEED")
    return String(ETH.linkSpeed());
  else if (var == "UPDATE")
    return updateStatus;

  return String();
}

void initializeServer() {
  if (!LITTLEFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LITTLEFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LITTLEFS, "/index.html", String(), false, processor);
  });

  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(readEEPROM(AdminUsernameStart, AdminUsernameLength), readEEPROM(AdminPasswordStart, AdminPasswordLength)))
      return request->requestAuthentication();
       
    if (request->hasParam("url")) {
      updateURL = String(request->getParam("url")->value());
      xTaskCreatePinnedToCore(updateTask, "updateTask", 10000, NULL, 1, &Task3, 1);  
    } else 
      updateStatus = "";

    if (request->hasParam("reboot"))
      ESP.restart();
    
    if (request->hasParam("topic"))
      writeEEPROM(MQTTTopicStart, MQTTTopicLength, (char*) request->getParam("topic")->value().c_str());

    if (request->hasParam("ipAddress"))
      writeEEPROM(MQTTIPStart, MQTTIPLength, (char*) request->getParam("ipAddress")->value().c_str());

    if (request->hasParam("port"))
      writeEEPROM(MQTTPortStart, MQTTPortLength, (char*) request->getParam("port")->value().c_str());

    if (request->hasParam("username"))
      writeEEPROM(MQTTUsernameStart, MQTTUsernameLength, (char*) request->getParam("username")->value().c_str());

    if (request->hasParam("password"))
      writeEEPROM(MQTTPasswordStart, MQTTPasswordLength, (char*) request->getParam("password")->value().c_str());

    if (request->hasParam("AdminUsername"))
      writeEEPROM(AdminUsernameStart, AdminUsernameLength, (char*) request->getParam("AdminUsername")->value().c_str());

    if (request->hasParam("AdminPassword"))
      writeEEPROM(AdminPasswordStart, AdminPasswordLength, (char*) request->getParam("AdminPassword")->value().c_str());

    if (request->hasParam("ipAddress") && request->hasParam("port") && request->hasParam("username") && request->hasParam("password"))
      client.disconnect();

    request->send(LITTLEFS, "/config.html", String(), false, processor);
  });

  server.on("/data.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LITTLEFS, "/data.xml", "application/xml", false, processor);
  });

  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LITTLEFS, "/data.json", "application/json", false, processor);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LITTLEFS, "/style.css", "text/css");
  });

  server.on("/sweetalert2@10", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LITTLEFS, "/sweetalert2@10", String());
  });

  server.begin();
}
