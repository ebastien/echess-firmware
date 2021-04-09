#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include <array>

#include "topo.h"
#include "footprint.h"

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

    Piece operator[](const Position& p) const { return b_[Topo::index(p)]; }
    Piece& operator[](const Position& p) { return b_[Topo::index(p)]; }

    bool isValidMove(const Position& from, const Position& to) const;
    bool isCastling(const Position& from, const Position& to) const;
    bool move(const Position& from, const Position& to);

    Array::const_iterator begin() const { return b_.begin(); }
    Array::const_iterator end()   const { return b_.end(); }
  };

}

#endif
