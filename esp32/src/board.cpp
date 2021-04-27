
#include "board.h"

using namespace echess;

Move Board::move(const Position& from, const Position& to) const {
  char tm[5];
  from.terse(tm); to.terse(tm + 2); tm[4] = 0;
  Move m;
  bool valid = m.m_.TerseIn(const_cast<thc::ChessRules*>(&cr_), tm);
  if (!valid) {
    m.m_.Invalid();
  }
  return m;
}

bool Move::isValid() const {
  return m_.Valid();
}

bool Move::isCastling() const {
  return m_.special == thc::SPECIAL_WK_CASTLING ||
         m_.special == thc::SPECIAL_BK_CASTLING ||
         m_.special == thc::SPECIAL_WQ_CASTLING ||
         m_.special == thc::SPECIAL_BQ_CASTLING;
}

bool Move::isEnPassant() const {
  return m_.special == thc::SPECIAL_WEN_PASSANT ||
         m_.special == thc::SPECIAL_BEN_PASSANT;
}

bool Move::isCapture() const {
  return m_.capture != ' ';
}

bool Board::doMove(const Move& m) {
  if (!m.isValid()) {
    return false;
  }
  cr_.PlayMove(m.m_);
  return true;
}
