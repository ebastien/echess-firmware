
#include "board.h"

using namespace echess;

Move Board::move(const Square& from, const Square& to) const {
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

Situation Board::situation() const {
  thc::TERMINAL t;
  const_cast<thc::ChessRules&>(cr_).Evaluate(t);
  switch (t) {
    case thc::TERMINAL_WCHECKMATE: return Situation::whiteCheckMate;
    case thc::TERMINAL_WSTALEMATE: return Situation::whiteStaleMate;
    case thc::TERMINAL_BCHECKMATE: return Situation::blackCheckMate;
    case thc::TERMINAL_BSTALEMATE: return Situation::whiteStaleMate;
    default:                       return Situation::ingame;
  }
}

bool Board::doMove(const Move& m) {
  if (!m.isValid()) {
    return false;
  }
  cr_.PlayMove(m.m_);
  return true;
}

Move Board::move(const char* terse) const {
  Square from(terse), to(terse+2);
  return move(from, to);
}

bool Board::fromMoves(const char* moves) {
  const int n = (strlen(moves) + 1) / 5;
  for (int i = 0; i < n; ++i) {
    const auto m = move(moves + i * 5);
    if (!doMove(m)) {
      return false;
    }
  }
  return true;
}
