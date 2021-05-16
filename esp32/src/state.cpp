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
  moves_.clear();
}

void Machine::move(const Move& m) {
  if (board_.doMove(m)) {
    moves_.push_back(m);
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

bool Machine::isReady() const {
  return std::visit(visitor {
    [](const StateWaiting&) { return true;  },
    [](const State&)        { return false; },
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

void Machine::moving(const StateMoving& s, const Change& c) {
  const auto uci = UCIMove(s.origin(), c.pos());
  if (c.dir() == Direction::placed) {
    // Placed
    if (s.origin() == c.pos()) {
      state_ = StateWaiting();
    } else {
      auto m = board_.move(uci);
      if (m.isValid()) {
        move(m);
        if (m.isCastling()) {
          state_ = StateCastling();
        } else if (m.isEnPassant()) {
          state_ = StatePassant();
        } else if (board_.isOver()) {
          state_ = StateOver(board_.situation());
        } else {
          state_ = StateWaiting();
        }
      } else {
        state_ = StateInvalid();
      }
    }
  } else {
    // Removed
    auto m = board_.move(uci);
    if (m.isValid() && m.isCapture()) {
      move(m);
      state_ = StateTaking();
    } else {
      state_ = StateInvalid();
    }
  }
}

void Machine::transition(const Footprint& f) {
  auto cs = compare(footprint_, f);
  footprint_ = f;
  if (!cs.empty()) {
    std::visit(visitor {
      [&](const StateInit&)     { syncing(); },
      [&](const StateWaiting&)  { if (cs.size() == 1) { waiting(cs.front());   } else { state_ = StateInvalid(); } },
      [&](const StateMoving& s) { if (cs.size() == 1) { moving(s, cs.front()); } else { state_ = StateInvalid(); } },
      [&](const StateCastling&) { syncing(); },
      [&](const StateTaking&)   { syncing(); },
      [&](const StatePassant&)  { syncing(); },
      [&](const StateInvalid&)  { syncing(); },
      [&](const StateOver&)     {}
    }, state_);
  }
}

void Machine::transition(const Moves& moves) {
  Board board;
  if (board.fromMoves(moves)) {
    board_ = board;
    state_ = StateInit();
    syncing();
  }
}

std::string Machine::explain() const {
  return std::visit(visitor {
    [](const StateInit&)     { return std::string("init"); },
    [](const StateInvalid&)  { return std::string("invalid pos"); },
    [](const StateWaiting&)  { return std::string("waiting..."); },
    [](const StateMoving& s) { return std::string("moving ") + s.origin().terse(); },
    [](const StateCastling&) { return std::string("castling"); },
    [](const StateTaking&)   { return std::string("taking"); },
    [](const StatePassant&)  { return std::string("en passant"); },
    [](const StateOver& s)   {
      switch (s.situation()) {
        case Situation::blackCheckMate: return std::string("black mate");
        case Situation::whiteCheckMate: return std::string("white mate");
        case Situation::blackStaleMate: return std::string("black stale");
        case Situation::whiteStaleMate: return std::string("white stale");
        default:                        return std::string("game over");
      }
    }
  }, state_);
}
