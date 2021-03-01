static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      ETH.setHostname((char*)ETH.macAddress().c_str());
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      eth_connected = false;
      break;
    default:
      break;
  }
}
