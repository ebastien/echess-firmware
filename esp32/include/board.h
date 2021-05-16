#ifndef _ECHESS_BOARD_H
#define _ECHESS_BOARD_H

#include <array>
#include <optional>

#include "thc.h"

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

  enum class Player {
    white = 0,
    black
  };

  enum class Situation {
    ingame = 0,
    whiteCheckMate,
    blackCheckMate,
    whiteStaleMate,
    blackStaleMate
  };

  struct PlayerPiece {
    Player player_ = Player::white;
    Piece piece_ = Piece::none;

    PlayerPiece() = default;
    PlayerPiece(const PlayerPiece&) = default;
    PlayerPiece& operator=(const PlayerPiece&) = default;

    PlayerPiece(const char c) {
      switch (c) {
        case 'k': player_ = Player::black; piece_ = Piece::king;   break;
        case 'q': player_ = Player::black; piece_ = Piece::queen;  break;
        case 'r': player_ = Player::black; piece_ = Piece::rook;   break;
        case 'b': player_ = Player::black; piece_ = Piece::bishop; break;
        case 'n': player_ = Player::black; piece_ = Piece::knight; break;
        case 'p': player_ = Player::black; piece_ = Piece::pawn;   break;
        case 'K': player_ = Player::white; piece_ = Piece::king;   break;
        case 'Q': player_ = Player::white; piece_ = Piece::queen;  break;
        case 'R': player_ = Player::white; piece_ = Piece::rook;   break;
        case 'B': player_ = Player::white; piece_ = Piece::bishop; break;
        case 'N': player_ = Player::white; piece_ = Piece::knight; break;
        case 'P': player_ = Player::white; piece_ = Piece::pawn;   break;
        default:  player_ = Player::white; piece_ = Piece::none;
      }
    }

    char symbol() const {
      if (player_ == Player::white) {
        switch (piece_) {
          case Piece::king:   return 'K';
          case Piece::queen:  return 'Q';
          case Piece::rook:   return 'R';
          case Piece::bishop: return 'B';
          case Piece::knight: return 'N';
          case Piece::pawn:   return 'P';
          default:            return ' ';
        }
      } else {
        switch (piece_) {
          case Piece::king:   return 'k';
          case Piece::queen:  return 'q';
          case Piece::rook:   return 'r';
          case Piece::bishop: return 'b';
          case Piece::knight: return 'n';
          case Piece::pawn:   return 'p';
          default:            return ' ';
        }
      }
    }
  };

  class Board;

  class Move {
    friend class Board;

    thc::Move m_;

    Move() {};

  public:
    Move(const Move&) = default;
    Move& operator=(const Move&) = default;

    Square from() const { return Square::fromForsyth(m_.src); }
    Square to() const { return Square::fromForsyth(m_.dst); }
    UCIMove uci() const { return UCIMove(const_cast<thc::Move&>(m_).TerseOut()); }

    bool isValid() const;
    bool isCastling() const;
    bool isEnPassant() const;
    bool isCapture() const;
  };

  class Board {
    thc::ChessRules cr_;
    std::vector<Move> moves_;

  public:
    Board() {}
    Board(const char* fen) { cr_.Forsyth(fen); }

    bool fromMoves(const UCIMoves& moves);

    PlayerPiece at(const Square& p) const { return PlayerPiece(cr_.squares[p.forsyth()]); }
    PlayerPiece operator[](const Square& p) const { return at(p); }
    Player player() const { return cr_.WhiteToPlay() ? Player::white : Player::black; }
    Move move(const UCIMove& uci) const;

    bool isEmpty(const Square& p) const { return at(p).piece_ == Piece::none; }
    bool isPlayer(const Square& p) const { return !isEmpty(p) && at(p).player_ == player(); }
    bool isOver() const { return situation() != Situation::ingame; }

    Situation situation() const;

    int length() const { return moves_.size(); }
    std::optional<Move> lastMove() const {
      return moves_.empty() ? std::nullopt : std::make_optional(moves_.back());
    }

    bool doMove(const Move& m);

    struct Iterator {
      using iterator_category = std::forward_iterator_tag;
      using value_type        = PlayerPiece;
      using reference         = PlayerPiece;
      using difference_type   = uint8_t;
      using pointer           = uint8_t;

      Iterator(const Board& board, const pointer index) : board_(board), index_(index) {}

      reference operator*() const { return board_.at(Square::fromNatural(index_)); }
      Iterator& operator++() { index_++; return *this; }
      Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
      friend bool operator==(const Iterator& a, const Iterator& b) { return a.index_ == b.index_; }
      friend bool operator!=(const Iterator& a, const Iterator& b) { return a.index_ != b.index_; }

    private:
      const Board& board_;
      pointer index_;
    };

    Iterator begin() const { return Iterator(*this, 0); }
    Iterator end()   const { return Iterator(*this, 64); }
  };
}

#endif
