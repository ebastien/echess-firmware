
#include "lichess.h"

#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>

using namespace echess;

std::string Lichess::baseURL() {
  return "https://lichess.org";
}
std::string Lichess::boardGameStreamURL(const gameid_t& gameId) {
  return std::string("https://lichess.org/api/board/game/stream/") + gameId;
}
std::string Lichess::accountPlayingURL() {
  return "https://lichess.org/api/account/playing";
}

bool Lichess::findGame() {

  if (client_.open(accountPlayingURL(), token_) < 0) {
    Serial.println("unable to connect to playing endpoint");
    return false;
  }
  Serial.println("connected to playing endpoint");

  StaticJsonDocument<768> doc;
  DeserializationError error(DeserializationError::Ok);

  error = deserializeJson(doc, client_);
  if (error) {
    Serial.print("deserialization failed: "); Serial.println(error.c_str());
    return false;
  }
  if (doc["nowPlaying"].size() == 0) {
    Serial.println("No game playing");
    return false;
  }
  const auto& game = doc["nowPlaying"][0];
  gameId_ = game["gameId"].as<gameid_t>();
  if (game["color"] == "black") {
    player_ = Player::black;
  } else {
    player_ = Player::white;
  }
  Serial.print("Game ID: "); Serial.println(gameId_.c_str());
  Serial.print("Player: "); Serial.println(player_ == Player::white ? "white" : "black");
  return true;
}

bool Lichess::readGameState() {

  if (!isGamePlaying()) {
    return false;
  }
  if (client_.open(boardGameStreamURL(gameId_), token_) < 0) {
    Serial.println("unable to connect to streaming endpoint");
    return false;
  }
  Serial.println("connected to steaming endpoint");

  StaticJsonDocument<1024> doc;
  DeserializationError error(DeserializationError::Ok);
  gamestate_t newState(gameState_);

  while (newState == gameState_ && !error) {

    Serial.println("read...");

    error = deserializeJson(doc, client_);
    while (error == DeserializationError::EmptyInput && client_.isConnected()) {
      delay(1000);
      error = deserializeJson(doc, client_);
    }

    if (!error) {
      if (doc["type"] == "gameFull") {
        newState = doc["state"]["moves"].as<gamestate_t>();
      } else if (doc["type"] == "gameState") {
        newState = doc["moves"].as<gamestate_t>();
      }
      Serial.print("Moves: "); Serial.println(newState.c_str());
    }
  }

  if (error) {
    Serial.print("deserialization failed: "); Serial.println(error.c_str());
  }
  if (newState == gameState_) {
    return false;
  }
  gameState_ = newState;
  return true;
}
