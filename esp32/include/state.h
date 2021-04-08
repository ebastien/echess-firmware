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
  public:
    StateMoving() {}
  };

  class StateCastling {
  public:
    StateCastling() {}
  };

  class StateTaking {
  public:
    StateTaking() {}
  };

  class StateInvalid {
  public:
    StateInvalid() {}
  };

  class Machine {
    using State = std::variant<StateInvalid, StateWaiting, StateMoving, StateCastling, StateTaking>;

    State state_;
    Board board_;

    void transition(const Change&);

  public:
    using BoardInput = std::vector<Change>;
    using UIInput = struct {};

    Machine(const Board& b) : state_(StateWaiting()), board_(b) {};

    void transition(const BoardInput&);
    void transition(const UIInput&);

    const Board& board() const { return board_; }
    const char* explain() const;
  };

}

#endif
