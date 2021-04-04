#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include "footprint.h"

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
  static const uint8_t c_axisSquares = Scanner::c_axisSquares;
  static const uint8_t c_boardSize = Scanner::c_boardSize;

private:
  Piece b_[c_boardSize];

public:
  Board();
};

#endif
