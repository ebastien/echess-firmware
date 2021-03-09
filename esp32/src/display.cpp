#include "display.h"
#include "footprint.h"

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
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_u8glib_4_hr);
  u8g2.setCursor(65, 10);
  u8g2.print(msg);
}

void Display::print(const int32_t n) {
  char buffer[20];
  itoa(n, buffer, 10);
  print(buffer);
}

void Display::print(const Footprint& board) {

  const uint8_t axisSquares = Footprint::c_axisSquares;
  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = axisSquares * squareDim;

  for (uint8_t y = 0; y < axisSquares; y++) {
    for (uint8_t x = 0; x < axisSquares; x++) {
      const u8g2_uint_t xs = x * squareDim;
      const u8g2_uint_t ys = boardDim - (y + 1) * squareDim;
      const uint8_t bgColor = (x + y) % 2;
      const uint8_t fgColor = (bgColor + 1) % 2;
      const bool square = board.at(x, y);

      u8g2.setDrawColor(bgColor);
      u8g2.drawBox(xs, ys, squareDim, squareDim);
      if (square) {
        u8g2.setDrawColor(fgColor);
        u8g2.drawDisc(xs + squareDim/2, ys + squareDim/2, squareDim/4);
      }
    }
  }
}
