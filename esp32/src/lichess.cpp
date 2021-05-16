
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
std::string Lichess::boardGameMoveURL(const gameid_t& gameId, const move_t& move) {
  return std::string("https://lichess.org/api/board/game/") + gameId + std::string("/move/") + move;
}

bool Lichess::findGame() {

  if (clientPlay_.open(accountPlayingURL(), token_) < 0) {
    clientPlay_.close();
    Serial.println("unable to connect to playing endpoint");
    return false;
  }
  Serial.println("connected to playing endpoint");

  StaticJsonDocument<768> doc;
  DeserializationError error(DeserializationError::Ok);

  error = deserializeJson(doc, clientPlay_);
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

bool Lichess::waitGameState(const int min) {

  if (!isGamePlaying()) {
    return false;
  }
  if (clientStream_.open(boardGameStreamURL(gameId_), token_) < 0) {
    clientStream_.close();
    Serial.println("unable to connect to streaming endpoint");
    return false;
  }
  Serial.println("connected to steaming endpoint");

  StaticJsonDocument<1024> doc;
  DeserializationError error(DeserializationError::Ok);
  Moves moves;

  do {
    Serial.println("read...");

    error = deserializeJson(doc, clientStream_);
    while (error == DeserializationError::EmptyInput && clientStream_.isConnected()) {
      delay(1000);
      error = deserializeJson(doc, clientStream_);
    }

    if (!error) {
      if (doc["type"] == "gameFull") {
        moves = doc["state"]["moves"].as<const char*>();
      } else if (doc["type"] == "gameState") {
        moves = doc["moves"].as<const char*>();
      }
      Serial.print("Moves: "); Serial.println(moves.uci().c_str());
    }
  }
  while (moves.length() <= min && !error);

  clientStream_.close();

  if (error) {
    Serial.print("deserialization failed: "); Serial.println(error.c_str());
    return false;
  }
  moves_ = moves;
  return true;
}

bool Lichess::makeMove(const Move& m) {
  if (!isGamePlaying()) {
    return false;
  }

  int err = clientPlay_.post(boardGameMoveURL(gameId_, m.uci().c_str()), token_);

  clientPlay_.close();

  if (err < 0) {
    Serial.println("unable to connect to move endpoint");
    return false;
  }
  moves_ += m.uci();
  Serial.println("connected to move endpoint");
  return true;
}
