#include "state.h"

void Machine::transition(const BoardInput& cs) {
  switch (cs.size()) {
    case 0: // no change
      break;
    case 1: // a single change
    {
      auto& c = cs.front();
      switch (s_.index()) {
        case 0: // from invalid state
          break;
        case 1: // from waiting
          if (c.dir() == Direction::removed) {
            s_ = StateMoving();
          } else {
            s_ = StateInvalid();
          }
          break;
        case 2: // from moving
          if (c.dir() == Direction::placed) {
            s_ = StateWaiting();
          } else {
            s_ = StateTaking();
          }
          break;
        case 3: // from castling
          break;
        case 4: // from taking
          if (c.dir() == Direction::placed) {
            s_ = StateWaiting();
          } else {
            s_ = StateInvalid();
          }
          break;
      }
      break;
    }
    default: // more than one change at a time
      s_ = StateInvalid();
  }
}

void Machine::transition(const UIInput& a) {
  // ...
}
