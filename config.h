#define VERSION __DATE__ " " __TIME__

#define MQTTIPStart 0
#define MQTTIPLength 32

#define MQTTUsernameStart 32
#define MQTTUsernameLength 16

#define MQTTPasswordStart 48
#define MQTTPasswordLength 32

#define MQTTPortStart 80
#define MQTTPortLength 16

#define MQTTTopicStart 96
#define MQTTTopicLength 16

#define AdminUsernameStart 112
#define AdminUsernameLength 16

#define AdminPasswordStart 128
#define AdminPasswordLength 16

#define UpdateCommandTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/update/cmnd").c_str()
#define UpdateStatusTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/update/stat").c_str()
#define CommandTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/cmnd").c_str()
#define WillTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/stat").c_str()
#define NetworkTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/network/stat").c_str()
#define DataTopic (char*) ("/" + String(readEEPROM(MQTTTopicStart, MQTTTopicLength)) + "/" + String(ETH.macAddress()) + "/data").c_str()

#include <ETH.h>
WiFiClient espClient;
