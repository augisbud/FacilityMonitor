#include <PubSubClient.h>
#include <ESP32httpUpdate.h>

PubSubClient client(espClient);

unsigned long previousMillis = 0;
const long interval = 60 * 1000;

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  if (String("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/FF:FF:FF:FF:FF:FF/cmnd") == String((char*)topic) || String(CommandTopic) == String((char*)topic) && String((char*) payload) == "reboot") {
    ESP.restart();
  } else if (String(CommandTopic) == String((char*)topic) && String((char*) payload) == "network") {
    String payload = "{ \"MAC\": \"" + ETH.macAddress() + "\",\"IPv4\": \"" + ETH.localIP().toString() + "\",\"Gateway\": \"" + ETH.gatewayIP().toString() + "\",\"SubnetMask\": \"" + ETH.subnetMask().toString() + "\",\"Link_speed\": \"" + ETH.linkSpeed() + "\" }";
    client.publish(NetworkTopic, (char*) payload.c_str());
  } else if (String(UpdateCommandTopic) == String((char*)topic) || String("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/FF:FF:FF:FF:FF:FF/update/cmnd") == String((char*)topic)) {
    t_httpUpdate_return ret = ESPhttpUpdate.update(String((char*)payload));

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        client.publish(UpdateStatusTopic, ("{ \"return\": \"HTTP_UPDATE_FAILED\", \"errorID\": \"" + String(ESPhttpUpdate.getLastError()) + "\", \"errorName\": \"" + String(ESPhttpUpdate.getLastErrorString()) + "\"}").c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        client.publish(UpdateStatusTopic, "{ \"return\": \"HTTP_UPDATE_NO_UPDATES\"}");
        break;

      case HTTP_UPDATE_OK:
        client.publish(UpdateStatusTopic, "{ \"return\": \"HTTP_UPDATE_OK\"}");
        break;
    }
  }
}

void TopicSubscribe() {
  client.subscribe(UpdateCommandTopic);
  client.subscribe(CommandTopic);

  client.subscribe( (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/FF:FF:FF:FF:FF:FF/update/cmnd").c_str() );
  client.subscribe( (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/FF:FF:FF:FF:FF:FF/cmnd").c_str() );
}

void reconnect() {
  while (!client.connected()) {
    client.setServer(readEEPROM(MQTTIPStart, MQTTIPLength), atoi(readEEPROM(MQTTPortStart, MQTTPortLength)));
    if (client.connect((char*) ETH.macAddress().c_str(), readEEPROM(MQTTUsernameStart, MQTTUsernameLength), readEEPROM(MQTTPasswordStart, MQTTPasswordLength), WillTopic, 0, true, "{ \"status\": \"Connection Lost\"}", true )) {
      client.publish(WillTopic, ("{ \"status\": \"Connected\" , \"version\": \"" + String(VERSION) + "\", \"started\": \" " + String(localTime()) + "\" }").c_str(), true);
      TopicSubscribe();
    }
  }
}

void DataPublish() {
  if (abs(getBME("temperature") - lastTemperature) > 0.5 || abs(getBME("humidity") - lastHumidity) > 1) {
    lastTemperature = getBME("temperature");
    lastHumidity = getBME("humidity");

    String payload = "{ \"temperature\": \"" + String(getBME("temperature")) + "\",\"humidity\": \"" + String(getBME("humidity")) + "\",\"pressure\": \"" + String(getBME("pressure")) + "\"}";
    client.publish(DataTopic, (char*) payload.c_str());
  }
}
