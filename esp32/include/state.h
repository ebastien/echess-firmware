#ifndef _ECHESS_STATE_H
#define _ECHESS_STATE_H

#include <variant>
#include <vector>

#include "board.h"
#include "footprint.h"

namespace echess {

  struct State {};

  struct StateInit     : public State {};
  struct StateWaiting  : public State {};
  struct StateCastling : public State {};
  struct StateTaking   : public State {};
  struct StatePassant  : public State {};
  struct StateInvalid  : public State {};

  struct StateOver : public State {
    Situation s_;

  public:
    StateOver(const Situation& s) : s_(s) {}

    const Situation& situation() const { return s_; }
  };

  class StateMoving : public State {
    Square origin_;

  public:
    StateMoving(const Square& o) : origin_(o) {}

    const Square& origin() const { return origin_; }
  };

  class Machine {
    using State = std::variant<StateInit, StateInvalid, StateWaiting, StateMoving, StateCastling, StateTaking, StatePassant, StateOver>;

    Footprint footprint_;
    State state_;
    Board board_;

    void move(const Move& m);

    void waiting(const Change& c);
    void moving(const StateMoving& s, const Change& c);
    void syncing();

  public:
    Machine();
    Machine(const Board& b) { reset(b); }

    void reset(const Board& b);
    void transition(const Footprint& f);
    void transition(const UCIMoves& moves);

    bool isValid() const;
    bool isReady() const;
    const Board& board() const { return board_; }
    const Footprint& footprint() const { return footprint_; }
    std::string explain() const;
  };

}

#endif
