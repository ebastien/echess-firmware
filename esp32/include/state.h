#ifndef _ECHESS_STATE_H
#define _ECHESS_STATE_H

#include <variant>
#include <vector>

#include "board.h"
#include "footprint.h"

namespace echess {

  struct StateWaiting {};
  struct StateCastling {};
  struct StateTaking {};
  struct StatePassant {};
  struct StateInvalid {};
  struct StateEnd {};

  class StateMoving {
    Square origin_;

  public:
    StateMoving(const Square& o) : origin_(o) {}

    const Square& origin() const { return origin_; }
  };

  class Machine {
    using State = std::variant<StateInvalid, StateWaiting, StateMoving, StateCastling, StateTaking, StatePassant>;

    Footprint footprint_;
    State state_;
    Board board_;

    void transition(const Change&);

  public:
    Machine(const Board& b) { reset(b); }

    void reset(const Board& b);
    void transition(const Footprint& f);

    bool isValid() const;
    const Board& board() const { return board_; }
    const Footprint& footprint() const { return footprint_; }
    const char* explain() const;
  };

}

#endif
