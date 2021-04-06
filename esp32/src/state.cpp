#include "state.h"
#include "helpers.h"

using namespace echess;

void Machine::transition(const Change& c) {
  std::visit(visitor {
    [&](const StateInvalid&) {},
    [&](const StateWaiting&) {
      if (c.dir() == Direction::removed) {
        // auto piece = board_.at(c.x(), c.y());
        state_ = StateMoving();
      } else {
        state_ = StateInvalid();
      }
    },
    [&](const StateMoving&) {
      if (c.dir() == Direction::placed) {
        state_ = StateWaiting();
      } else {
        state_ = StateTaking();
      }
    },
    [&](const StateCastling&) {},
    [&](const StateTaking&) {
      if (c.dir() == Direction::placed) {
        state_ = StateWaiting();
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
