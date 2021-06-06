
#include "board.h"

using namespace echess;

bool Move::isValid() const {
  return m_.Valid();
}

bool Move::isPromotion() const {
  return m_.special == thc::SPECIAL_PROMOTION_QUEEN ||
         m_.special == thc::SPECIAL_PROMOTION_ROOK ||
         m_.special == thc::SPECIAL_PROMOTION_BISHOP ||
         m_.special == thc::SPECIAL_PROMOTION_KNIGHT;
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

UCIMove Move::uci960() const {
  UCIMove m(uci());
  if (isCastling()) {
    if (m == UCIMove("e1g1")) return UCIMove("e1h1");
    if (m == UCIMove("e1c1")) return UCIMove("e1a1");
    if (m == UCIMove("e8g8")) return UCIMove("e8h8");
    if (m == UCIMove("e8c8")) return UCIMove("e8a8");
  }
  return m;
}

UCIMove Board::fromChess960(const UCIMove& uci) const {
  const PlayerPiece p = at(uci.from());
  if (p.piece_ == Piece::king) {
    if (p.player_ == Player::white) {
      if (uci == UCIMove("e1h1")) return UCIMove("e1g1");
      if (uci == UCIMove("e1a1")) return UCIMove("e1c1");
    } else {
      if (uci == UCIMove("e8h8")) return UCIMove("e8g8");
      if (uci == UCIMove("e8a8")) return UCIMove("e8c8");
    }
  }
  return uci;
}

Move Board::move(const UCIMove& uci) const {
  Move m;
  bool valid = m.m_.TerseIn(const_cast<thc::ChessRules*>(&cr_), fromChess960(uci).c_str());
  if (!valid) {
    m.m_.Invalid();
  }
  return m;
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
  moves_.push_back(m);
  return true;
}

bool Board::fromMoves(const UCIMoves& moves) {
  for (const auto& m : moves) {
    if (!doMove(move(m))) {
      return false;
    }
  }
  return true;
}
