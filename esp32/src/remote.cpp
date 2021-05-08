
#include "remote.h"

#include <WiFi.h>

using namespace echess;

void Remote::connect(const char* ssid, const char* pass) {
  if (WiFi.isConnected()) {
    return;
  }
  WiFi.begin(ssid, pass);
  while(!WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
