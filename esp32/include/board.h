#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include <array>

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
    using Array = std::array<Piece, Topo::c_boardSize>;

    Array b_ = {Piece::none};

  public:
    Board() {}

    Piece& operator[](const Position& p) { return b_[Topo::index(p)]; }
    Piece operator[](const Position& p) const { return b_[Topo::index(p)]; }

    Array::iterator begin() { return b_.begin(); }
    Array::iterator end()   { return b_.end(); }
  };

}

#endif
