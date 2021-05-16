
#include "board.h"

using namespace echess;

Move Board::move(const UCIMove& uci) const {
  Move m;
  bool valid = m.m_.TerseIn(const_cast<thc::ChessRules*>(&cr_), uci.c_str());
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

bool Board::fromMoves(const Moves& moves) {
  for (const auto& m : moves) {
    if (!doMove(move(m))) {
      return false;
    }
  }
  return true;
}
