#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include <stdint.h>

const uint8_t axisSquares = 8;
const uint8_t boardSize = axisSquares * axisSquares;

enum Piece {
  none = 0,
  king,
  queen,
  rook,
  bishop,
  knight,
  pawn
};

typedef Piece Board[boardSize];

#endif