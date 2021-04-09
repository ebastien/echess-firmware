#ifndef _ECHESS_STATE_H
#define _ECHESS_STATE_H

#include <variant>
#include <vector>

#include "board.h"
#include "footprint.h"

namespace echess {

  class StateWaiting {
  public:
    StateWaiting() {}
  };

  class StateMoving {
    Position origin_;

  public:
    StateMoving(const Position& o) : origin_(o) {}

    const Position& origin() const { return origin_; }
  };

  class StateCastling {
  public:
    StateCastling() {}
  };

  class StateTaking {
    Position from_, to_;

  public:
    StateTaking(const Position& from, const Position& to) : from_(from), to_(to) {}

    const Position& from() const { return from_; }
    const Position& to() const { return to_; }
  };

  class StateInvalid {
  public:
    StateInvalid() {}
  };

  class Machine {
    using State = std::variant<StateInvalid, StateWaiting, StateMoving, StateCastling, StateTaking>;

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
