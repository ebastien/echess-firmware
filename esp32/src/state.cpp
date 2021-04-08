#include "state.h"
#include "helpers.h"

using namespace echess;

void Machine::transition(const Change& c) {
  std::visit(visitor {
    [&](const StateInvalid&) {},
    [&](const StateWaiting&) {
      if (c.dir() == Direction::removed) {
        if (board_[c.pos()] == Piece::none) {
          state_ = StateInvalid();
        } else {
          state_ = StateMoving(c.pos());
        }
      } else {
        state_ = StateInvalid();
      }
    },
    [&](const StateMoving& s) {
      if (c.dir() == Direction::placed) {
        if (board_[c.pos()] != Piece::none) {
          state_ = StateInvalid();
        } else {
          if (board_.move(s.origin(), c.pos())) {
            state_ = StateWaiting();
          } else {
            state_ = StateInvalid();
          }
        }
      } else {
        if (board_[c.pos()] == Piece::none) {
          state_ = StateInvalid();
        } else {
          if (board_.isValidMove(s.origin(), c.pos())) {
            state_ = StateTaking(s.origin(), c.pos());
          } else {
            state_ = StateInvalid();
          }
        }
      }
    },
    [&](const StateCastling&) {},
    [&](const StateTaking& s) {
      if (c.dir() == Direction::placed) {
        if (c.pos() == s.to()) {
          if (board_.move(s.from(), s.to())) {
            state_ = StateWaiting();
          } else {
            state_ = StateInvalid();
          }
        } else {
          state_ = StateInvalid();
        }
      } else {
        state_ = StateInvalid();
      }
    },
  }, state_);
}

void Machine::transition(const BoardInput& cs) {
  switch (cs.size()) {
    case 0: // no change
      break;
    case 1: // a single change
      transition(cs.front());
      break;
    default: // more than one change at a time
      state_ = StateInvalid();
  }
}

void Machine::transition(const UIInput& a) {
  // ...
}

const char* Machine::explain() const {
  return std::visit(visitor {
    [](const StateInvalid&)  { return "invalid";  },
    [](const StateWaiting&)  { return "waiting";  },
    [](const StateMoving&)   { return "moving";   },
    [](const StateCastling&) { return "castling"; },
    [](const StateTaking&)   { return "taking";   }
  }, state_);
}
