
#include "storage.h"

#include <Arduino.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>

using namespace echess;

bool Storage::initialize() {
  static const char* fname = "/echess.json";

  if(!SD.begin()){
    Serial.println("SD card initialization failed");
    return false;
  }
  if(SD.cardType() == CARD_NONE){
    Serial.println("no SD card attached");
    return false;
  }
  if (!SD.exists(fname)) {
    Serial.println("no configuration file found");
    return false;
  }
  File file = SD.open(fname);
  if (!file) {
    Serial.println("could not open configuration file");
    return false;
  }

  StaticJsonDocument<768> doc;
  DeserializationError error(DeserializationError::Ok);
  error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("deserialization failed: "); Serial.println(error.c_str());
    return false;
  }

  ssid_ = doc["ssid"].as<std::string>();
  password_ = doc["password"].as<std::string>();
  lichessKey_ = doc["lichessKey"].as<std::string>();
  return true;
}
