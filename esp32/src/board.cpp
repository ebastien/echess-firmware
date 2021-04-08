#include "board.h"

using namespace echess;

bool Board::isValidMove(const Position& from, const Position& to) const {
  return true;
}

bool Board::move(const Position& from, const Position& to) {
  const uint8_t f = Topo::index(from);
  const uint8_t t = Topo::index(to);
  b_[t] = b_[f];
  b_[f] = Piece::none;
  return true;
}
