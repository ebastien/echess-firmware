#include "state.h"
#include "helpers.h"

using namespace echess;

void Machine::reset(const Board& b) {
  state_ = StateWaiting();
  board_ = b;
  for (const auto p : Topo()) {
    footprint_[p] = (board_[p] != Piece::none);
  }
}

bool Machine::isValid() const {
  for (const auto p : Topo()) {
    if (footprint_[p] != (board_[p] != Piece::none)) {
      return false;
    }
  }
  return true;
}

void Machine::transition(const Change& c) {
  std::visit(visitor {
    [&](const StateInvalid&) {
      if (isValid()) {
        state_ = StateWaiting();
      }
    },
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
          if (board_.isCastling(s.origin(), c.pos())) {
            state_ = StateCastling();
          } else {
            if (board_.move(s.origin(), c.pos())) {
              state_ = StateWaiting();
            } else {
              state_ = StateInvalid();
            }
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
    [&](const StateCastling&) {
      if (isValid()) {
        state_ = StateWaiting();
      }
    },
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
    [](const StateTaking&)   { return "taking";   }
  }, state_);
}
