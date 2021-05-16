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
    Moves moves_;

    Lichess() : clientStream_(baseURL()), clientPlay_(baseURL()) { reset(); }

  public:
    Lichess(const Lichess&) = delete;
    Lichess& operator=(const Lichess&) = delete;

    static Lichess& getInstance() {
      static Lichess instance;
      return instance;
    }

    const Moves& moves() const { return moves_; }
    Player player() const { return player_; }

    void setToken(const std::string& token) { token_ = token; }
    void reset() { gameId_.clear(); moves_.clear(); }
    bool isGamePlaying() const { return !gameId_.empty(); }
    bool findGame();
    bool waitGameState(const int min);
    bool makeMove(const Move& m);
    void close() { clientStream_.close(); clientPlay_.close(); }
  };
}

#endif
