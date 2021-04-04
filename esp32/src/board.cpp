#include "board.h"

Board::Board() {
  for (uint8_t n = 0; n < c_boardSize; n++) {
    b_[n] = Piece::none;
  }
}
