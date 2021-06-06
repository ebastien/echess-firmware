#ifndef _ECHESS_STATE_H
#define _ECHESS_STATE_H

#include <variant>
#include <vector>

#include "board.h"
#include "footprint.h"

namespace echess {

  struct State {};

  struct StateInit      : public State {};
  struct StateWaiting   : public State {};
  struct StateCastling  : public State {};
  struct StateTaking    : public State {};
  struct StatePassant   : public State {};
  struct StateInvalid   : public State {};
  struct StatePromoting : public State {};

  struct StateOver : public State {
    Situation s_;

  public:
    explicit StateOver(const Situation& s) : s_(s) {}

    const Situation& situation() const { return s_; }
  };

  class StateMoving : public State {
    Square origin_;

  public:
    explicit StateMoving(const Square& o) : origin_(o) {}

    Square origin() const { return origin_; }
  };

  class StatePickPromotion : public State {
    Square from_, to_;

  public:
    explicit StatePickPromotion(const Square& from, const Square& to) : from_(from), to_(to) {}

    Square from() const { return from_; }
    Square to() const { return to_; }
  };

  class Machine {
    using State = std::variant<
      StateInit,
      StateInvalid,
      StateWaiting,
      StateMoving,
      StateCastling,
      StateTaking,
      StatePassant,
      StateOver,
      StatePickPromotion,
      StatePromoting
    >;

    Footprint footprint_;
    State state_;
    Board board_;

    void waiting(const Change& c);
    void moving(const StateMoving& s, const Change& c);
    void syncing();
    void promoting(const StatePickPromotion& s, const UCIMove::Promotion p);

  public:
    Machine();
    explicit Machine(const Board& b) { reset(b); }

    void reset(const Board& b);
    bool transition(const Footprint& f);
    bool transition(const UCIMove::Promotion p);
    bool transition(const UCIMoves& moves);

    bool isValid() const;
    bool isReady() const;
    bool isError() const;
    bool isPromotion() const;
    const Board& board() const { return board_; }
    const Footprint& footprint() const { return footprint_; }
    std::string explain() const;
  };

}

#endif
