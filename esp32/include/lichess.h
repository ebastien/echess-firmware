#ifndef _ECHESS_LICHESS_H
#define _ECHESS_LICHESS_H

#include "httpclient.h"
#include "board.h"

#include <string>

namespace echess {

  struct Lichess {
    using gameid_t    = std::string;
    using move_t      = std::string;

  private:
    static std::string baseURL();
    static std::string boardGameStreamURL(const gameid_t& gameId);
    static std::string accountPlayingURL();
    static std::string boardGameMoveURL(const gameid_t& gameId, const move_t& move);

    std::string token_;
    HTTPClient clientStream_, clientPlay_;
    gameid_t gameId_;
    Player player_;

    Lichess() : clientStream_(baseURL()), clientPlay_(baseURL()) { reset(); }

  public:
    Lichess(const Lichess&) = delete;
    Lichess& operator=(const Lichess&) = delete;

    static Lichess& getInstance() {
      static Lichess instance;
      return instance;
    }

    Player player() const { return player_; }

    void setToken(const std::string& token) { token_ = token; }
    void reset() { gameId_.clear(); }
    bool isGamePlaying() const { return !gameId_.empty(); }
    bool findGame();
    std::optional<UCIMoves> waitGameState(const int min = -1);
    bool makeMove(const UCIMove& m);
    void close() { clientStream_.close(); clientPlay_.close(); }
  };
}

#endif
