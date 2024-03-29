#include "state.h"
#include "helpers.h"

using namespace echess;

Machine::Machine() {
  reset(Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
}

void Machine::reset(const Board& b) {
  state_ = StateInit();
  footprint_ = Footprint();
  board_ = b;
}

bool Machine::isValid() const {
  for (const auto p : Topo()) {
    if (footprint_[p] == board_.isEmpty(p)) {
      return false;
    }
  }
  return true;
}

bool Machine::isReady() const {
  return std::visit(visitor {
    [](const StateWaiting&) { return true;  },
    [](const State&)        { return false; },
  }, state_);
}

bool Machine::isError() const {
  return std::visit(visitor {
    [](const StateInvalid&) { return true;  },
    [](const State&)        { return false; },
  }, state_);
}

bool Machine::isPromotion() const {
  return std::visit(visitor {
    [](const StatePickPromotion&) { return true;  },
    [](const State&)              { return false; },
  }, state_);
}

void Machine::syncing() {
  if (isValid()) {
    if (board_.isOver()) {
      state_ = StateOver(board_.situation());
    } else {
      state_ = StateWaiting();
    }
  }
}

void Machine::waiting(const Change& c) {
  if (c.dir() == Direction::removed && board_.isPlayer(c.pos())) {
    state_ = StateMoving(c.pos());
  } else {
    state_ = StateInvalid();
  }
}

void Machine::promoting(const StatePickPromotion& s, const UCIMove::Promotion p) {
  const UCIMove uci(s.from(), s.to(), p);
  const auto m = board_.move(uci);
  if (m.isValid()) {
    board_.doMove(m);
    if (isValid()) {
      state_ = StateWaiting();
    } else {
      state_ = StatePromoting();
    }
  } else {
    state_ = StateInvalid();
  }
}

void Machine::moving(const StateMoving& s, const Change& c) {

  const UCIMove uci(s.origin(), c.pos());

  if (c.dir() == Direction::placed) {
    // Placed
    if (s.origin() == c.pos()) {
      state_ = StateWaiting();
    } else {
      const auto m = board_.move(uci);
      if (m.isValid()) {
        if (m.isPromotion()) {
          state_ = StatePickPromotion(s.origin(), c.pos());
        } else {
          board_.doMove(m);
          if (m.isCastling()) {
            state_ = StateCastling();
          } else if (m.isEnPassant()) {
            state_ = StatePassant();
          } else if (board_.isOver()) {
            state_ = StateOver(board_.situation());
          } else {
            state_ = StateWaiting();
          }
        }
      } else {
        state_ = StateInvalid();
      }
    }
  } else {
    // Removed
    const auto m = board_.move(uci);
    if (m.isValid() && m.isCapture()) {
      if (m.isPromotion()) {
        state_ = StatePickPromotion(s.origin(), c.pos());
      } else {
        board_.doMove(m);
        state_ = StateTaking();
      }
    } else {
      state_ = StateInvalid();
    }
  }
}

bool Machine::transition(const Footprint& f) {
  auto cs = compare(footprint_, f);
  footprint_ = f;
  std::visit(visitor {
    [&](const StateInit&)        { syncing(); },
    [&](const StateWaiting&)     {
      if (!cs.empty()) {
        if (cs.size() == 1) {
          waiting(cs.front());
        } else {
          state_ = StateInvalid();
        }
      }
    },
    [&](const StateMoving& s)    {
      if (!cs.empty()) {
        if (cs.size() == 1) {
          moving(s, cs.front());
        } else {
          state_ = StateInvalid();
        }
      }
    },
    [&](const StatePromoting&)   { syncing(); },
    [&](const StateCastling&)    { syncing(); },
    [&](const StateTaking&)      { syncing(); },
    [&](const StatePassant&)     { syncing(); },
    [&](const StateInvalid&)     { syncing(); },
    [&](const State&)            {}
  }, state_);
  return true;
}

bool Machine::transition(const UCIMove::Promotion p) {
  return std::visit(visitor {
    [&](const StatePickPromotion& s) { promoting(s, p); return true; },
    [&](const State&)                {                  return false; }
  }, state_);
}

bool Machine::transition(const UCIMoves& moves) {
  if (moves.length() < board_.length()) {
    return false;
  }
  int n = 0;
  for (; n < board_.length(); ++n) {
    const auto& m = board_.moveAt(n);
    const auto& uci = moves.at(n);
    if (m.uci() != uci && m.uci960() != uci) {
      return false;
    }
  }
  for (; n < moves.length(); ++n) {
    const auto m = board_.move(moves.at(n));
    if (!board_.doMove(m)) {
      return false;
    }
  }
  state_ = StateInit();
  syncing();
  return true;
}

std::string Machine::explain() const {
  return std::visit(visitor {
    [](const StateInit&)          { return std::string("set position"); },
    [](const StateInvalid&)       { return std::string("invalid move"); },
    [](const StateWaiting&)       { return std::string("waiting for move"); },
    [](const StateMoving& s)      { return std::string("moving ") + s.origin().terse(); },
    [](const StateCastling&)      { return std::string("castling"); },
    [](const StateTaking&)        { return std::string("taking"); },
    [](const StatePassant&)       { return std::string("en passant"); },
    [](const StatePickPromotion&) { return std::string("ask promotion"); },
    [](const StatePromoting&)     { return std::string("promoting"); },
    [](const StateOver& s)   {
      switch (s.situation()) {
        case Situation::blackCheckMate: return std::string("black checkmate");
        case Situation::whiteCheckMate: return std::string("white checkmate");
        case Situation::blackStaleMate: return std::string("black stalemate");
        case Situation::whiteStaleMate: return std::string("white stalemate");
        default:                        return std::string("game over");
      }
    }
  }, state_);
}
