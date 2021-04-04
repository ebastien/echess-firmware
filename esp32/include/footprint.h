#ifndef _ECHESS_FOOTPRINT_H
#define _ECHESS_FOOTPRINT_H

#include <vector>

#include <Adafruit_MCP23017.h>

class Footprint;
class Scanner {
  static const uint8_t c_mcpSquaresUnits = 4;
  static const uint8_t c_mcpGPIOs = 16;
  static const uint8_t c_gpioInterrupt = 33;
  static const unsigned long c_bouncingClearance = 500000; // 500ms
  static const uint8_t c_mcpUnitLayout[c_mcpSquaresUnits];

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

enum class Direction {
  removed = 0,
  placed
};

class Change {
  uint8_t x_, y_;
  Direction d_;

public:
  Change(uint8_t x, uint8_t y, Direction d): x_(x), y_(y), d_(d) {}

  uint8_t x() const { return x_; }
  uint8_t y() const { return y_; }
  Direction dir() const { return d_; }
};

class Footprint {
  friend Scanner;

public:
  static const uint8_t c_axisSquares = Scanner::c_axisSquares;
  static const uint8_t c_boardSize = Scanner::c_boardSize;

private:
  bool fp_[c_boardSize];

public:
  Footprint();

  bool at(uint8_t x, uint8_t y) const { return fp_[y * c_axisSquares + x]; }

  std::vector<Change> compare(const Footprint&) const;
};

#endif
