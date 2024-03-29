#include "display.h"
#include "footprint.h"

using namespace echess;

Display::Display() : u8g2(U8G2_R0, U8X8_PIN_NONE) {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void Display::prepare() {
  u8g2.clearBuffer();
}

void Display::draw() {
  u8g2.sendBuffer();
}

void Display::print(const char *msg) {
  u8g2.setDrawColor(1);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_8x13_tf);
  u8g2.setCursor(0, 22);
  u8g2.print(msg);
}

void Display::print(const std::string& s) {
  print(s.c_str());
}

void Display::print(const int32_t n) {
  char buffer[20];
  itoa(n, buffer, 10);
  print(buffer);
}

void Display::print(const Footprint& board) {
  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = 8 * squareDim;

  for (auto p : Topo()) {
    const u8g2_uint_t xs = boardDim + p.x() * squareDim;
    const u8g2_uint_t ys = boardDim - (p.y() + 1) * squareDim;
    const uint8_t bgColor = (p.x() + p.y()) % 2;
    const uint8_t fgColor = (bgColor + 1) % 2;
    const bool square = board[p];

    u8g2.setDrawColor(bgColor);
    u8g2.drawBox(xs, ys, squareDim, squareDim);
    if (square) {
      u8g2.setDrawColor(fgColor);
      u8g2.drawDisc(xs + squareDim/2, ys + squareDim/2, squareDim/4);
    }
  }
  u8g2.setDrawColor(0);
  u8g2.drawLine(64, 0, 64, 63);
}

void Display::print(const Board& board) {
  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = 8 * squareDim;

  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_5x7_tf);

  for (auto p : Topo()) {
    const u8g2_uint_t xs = p.x() * squareDim;
    const u8g2_uint_t ys = boardDim - (p.y() + 1) * squareDim;
    const uint8_t bgColor = (p.x() + p.y()) % 2;
    const uint8_t fgColor = (bgColor + 1) % 2;
    const PlayerPiece& pp = board[p];

    u8g2.setDrawColor(bgColor);
    u8g2.drawBox(xs, ys, squareDim, squareDim);
    if (pp.piece_ != Piece::none) {
      u8g2.setDrawColor(fgColor);
      u8g2.setCursor(xs + 2, ys + squareDim - 1);
      u8g2.print(pp.symbol());
    }
  }
  u8g2.setDrawColor(1);
  u8g2.drawLine(63, 0, 63, 63);
}

void Display::print(const UCIMove& move) {
  u8g2.setDrawColor(1);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_10x20_tf);
  u8g2.setCursor(68, 22);
  u8g2.print(move.c_str());
}

void Display::print(const UCIMove::Promotion p) {
  const char* txt = "";
  switch (p) {
    case UCIMove::queen:  txt = "Queen?";  break;
    case UCIMove::rook:   txt = "Rook?";   break;
    case UCIMove::bishop: txt = "Bishop?"; break;
    case UCIMove::knight: txt = "Knight?";
  }
  u8g2.setDrawColor(1);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_10x20_tf);
  u8g2.setCursor(32, 40);
  u8g2.print(txt);
}