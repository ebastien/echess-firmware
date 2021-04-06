#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include "topo.h"

namespace echess {

enum class Piece {
  none = 0,
  king,
  queen,
  rook,
  bishop,
  knight,
  pawn
};
class Board {
public:
  static const uint8_t c_axisSquares = Topo::c_axisSquares;
  static const uint8_t c_boardSize = Topo::c_boardSize;

private:
  Piece b_[c_boardSize] = {Piece::none};

public:
  Board() {}

  Piece at(uint8_t x, uint8_t y) const { return b_[y * c_axisSquares + x]; }
  Piece at(const Position& p) const { return b_[p.y() * c_axisSquares + p.x()]; }
};

}

#endif
