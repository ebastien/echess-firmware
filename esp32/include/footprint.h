#ifndef _ECHESS_FOOTPRINT_H
#define _ECHESS_FOOTPRINT_H

#include <Adafruit_MCP23017.h>

class Footprint;
class Scanner {
  static const uint8_t c_mcpSquaresUnits = 4;
  static const uint8_t c_mcpGPIOs = 16;
  static const uint8_t c_gpioInterrupt = 33;
  static const unsigned long c_bouncingClearance = 500000; // 500ms

  static volatile bool awakenByInterrupt;
  static void interruptCallback();

  Adafruit_MCP23017 s_mcps[c_mcpSquaresUnits];

  Scanner();

public:
  static const uint8_t c_axisSquares = 8;
  static const uint8_t c_boardSize = c_axisSquares * c_axisSquares;

  Scanner(const Scanner&) = delete;
  Scanner& operator=(const Scanner&) = delete;

  static Scanner& getInstance() {
    static Scanner instance;
    return instance;
  }

  void waitForInterrupt();
  void clearInterrupt();
  bool read(Footprint& fp);
  void debounce(Footprint& fp);
};

class Footprint {
  friend Scanner;

  bool fp_[Scanner::c_boardSize];

public:
  static const uint8_t c_axisSquares = Scanner::c_axisSquares;
  static const uint8_t c_boardSize = Scanner::c_boardSize;

  Footprint();

  bool at(uint8_t x, uint8_t y) const { return fp_[y * c_axisSquares + x]; }
};

#endif
