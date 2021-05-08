#include <Arduino.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>

void i2cInit()
{
  Wire.begin();

  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC);
  Serial.println(" device(s).");
}

void sdInit() {
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  const char* fname = "/echess.json";

  if (!SD.exists(fname)) {
    Serial.println("No configuration file found");
    return;
  }
  File file = SD.open(fname);
  if (!file) {
    Serial.println("Could not open configuration file");
    return;
  }
  Serial.println("Configuration file opened");

  StaticJsonDocument<768> doc;
  DeserializationError error(DeserializationError::Ok);
  error = deserializeJson(doc, file);
  if (error) {
    Serial.print("deserialization failed: "); Serial.println(error.c_str());
  } else {
    auto ssid = doc["ssid"].as<std::string>();
    auto password = doc["password"].as<std::string>();
    auto apiKey = doc["lichessKey"].as<std::string>();
    Serial.println(ssid.c_str());
    Serial.println(password.c_str());
    Serial.println(apiKey.c_str());
  }

  file.close();
}
