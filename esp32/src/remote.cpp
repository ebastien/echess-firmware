
#include "remote.h"

#include <WiFi.h>

using namespace echess;

void Remote::connect() {
  if (WiFi.isConnected()) {
    return;
  }
  WiFi.begin(ssid_.c_str(), pass_.c_str());
  while(!WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
