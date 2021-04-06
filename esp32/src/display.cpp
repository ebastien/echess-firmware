#include "display.h"
#include "footprint.h"

using namespace echess;

Display::Display() : u8g2(U8G2_R0, U8X8_PIN_NONE), row(0) {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void Display::prepare() {
  row = 0;
  u8g2.clearBuffer();
}

void Display::draw() {
  u8g2.sendBuffer();
}

void Display::print(const char *msg) {
  u8g2.setDrawColor(1);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_u8glib_4_hr);
  u8g2.setCursor(65, 10 + row * 5);
  u8g2.print(msg);
  row++;
}

void Display::print(const int32_t n) {
  char buffer[20];
  itoa(n, buffer, 10);
  print(buffer);
}

void Display::print(const Footprint& board) {
  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = Topo::c_axisSquares * squareDim;

  for (auto p : Topo()) {
    const u8g2_uint_t xs = p.x() * squareDim;
    const u8g2_uint_t ys = boardDim - (p.y() + 1) * squareDim;
    const uint8_t bgColor = (p.x() + p.y()) % 2;
    const uint8_t fgColor = (bgColor + 1) % 2;
    const bool square = board.at(p);

    u8g2.setDrawColor(bgColor);
    u8g2.drawBox(xs, ys, squareDim, squareDim);
    if (square) {
      u8g2.setDrawColor(fgColor);
      u8g2.drawDisc(xs + squareDim/2, ys + squareDim/2, squareDim/4);
    }
  }
}
