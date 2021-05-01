#include "state.h"
#include "helpers.h"

using namespace echess;

void Machine::reset(const Board& b) {
  state_ = StateInvalid();
  board_ = b;
  for (const auto p : Topo()) {
    footprint_.mark(p, !board_.isEmpty(p));
  }
}

bool Machine::isValid() const {
  for (const auto p : Topo()) {
    if (footprint_[p] == board_.isEmpty(p)) {
      return false;
    }
  }
  return true;
}

void Machine::transition(const Change& c) {
  std::visit(visitor {
    [&](const StateWaiting&) {
      if (c.dir() == Direction::removed && board_.isPlayer(c.pos())) {
        state_ = StateMoving(c.pos());
      } else {
        state_ = StateInvalid();
      }
    },
    [&](const StateMoving& s) {
      if (c.dir() == Direction::placed) {
        // Placed
        if (!board_.isEmpty(c.pos())) {
          // Placed over existing piece ?!
          state_ = StateInvalid();
        } else {
          auto m = board_.move(s.origin(), c.pos());
          if (m.isValid()) {
            board_.doMove(m);
            if (m.isCastling()) {
              state_ = StateCastling();
            } else if (m.isEnPassant()) {
              state_ = StatePassant();
            } else {
              state_ = StateWaiting();
            }
          } else {
            state_ = StateInvalid();
          }
        }
      } else {
        // Removed
        if (board_.isEmpty(c.pos())) {
          // Removed from empty position ?!
          state_ = StateInvalid();
        } else {
          auto m = board_.move(s.origin(), c.pos());
          if (m.isValid() && m.isCapture()) {
            board_.doMove(m);
            state_ = StateTaking();
          } else {
            state_ = StateInvalid();
          }
        }
      }
    },
    [&](const StateCastling&) { if (isValid()) { state_ = StateWaiting(); } },
    [&](const StateTaking&)   { if (isValid()) { state_ = StateWaiting(); } },
    [&](const StatePassant&)  { if (isValid()) { state_ = StateWaiting(); } },
    [&](const StateInvalid&)  { if (isValid()) { state_ = StateWaiting(); } },
  }, state_);
}

void Machine::transition(const Footprint& f) {
  auto cs = compare(footprint_, f);
  footprint_ = f;
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

const char* Machine::explain() const {
  return std::visit(visitor {
    [](const StateInvalid&)  { return "invalid";  },
    [](const StateWaiting&)  { return "waiting";  },
    [](const StateMoving&)   { return "moving";   },
    [](const StateCastling&) { return "castling"; },
    [](const StateTaking&)   { return "taking";   },
    [](const StatePassant&)  { return "passant";  },
    [](const StateEnd&)      { return "end";      }
  }, state_);
}
