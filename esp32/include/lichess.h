#ifndef _ECHESS_LICHESS_H
#define _ECHESS_LICHESS_H

#include "httpclient.h"

#include <string>

namespace echess {

  struct Lichess {
    using gameid_t    = std::string;
    using gamestate_t = std::string;

  private:
    static std::string baseURL();
    static std::string boardGameStreamURL(const gameid_t& gameId);
    static std::string accountPlayingURL();

    std::string token_;
    HTTPClient client_;
    gameid_t gameId_;
    gamestate_t gameState_;

    Lichess() : client_(baseURL()) { reset(); }

  public:
    Lichess(const Lichess&) = delete;
    Lichess& operator=(const Lichess&) = delete;

    static Lichess& getInstance() {
      static Lichess instance;
      return instance;
    }

    gamestate_t state() const { return gameState_; }

    void setToken(const std::string& token) { token_ = token; }
    void reset() { gameId_.clear(); gameState_.clear(); }
    bool isGamePlaying() const { return !gameId_.empty(); }
    bool findGame();
    bool readGameState();
    void close() { client_.close(); }
  };
}

#endif
