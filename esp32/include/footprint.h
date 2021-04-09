#ifndef _ECHESS_FOOTPRINT_H
#define _ECHESS_FOOTPRINT_H

#include "topo.h"

#include <vector>
#include <array>

#include <Adafruit_MCP23017.h>

namespace echess {

  enum class Direction {
    removed = 0,
    placed
  };

  class Change {
    Position p_;
    Direction d_;

  public:
    Change(const Position& p, const Direction d): p_(p), d_(d) {}

    const Position& pos() const { return p_; }
    const Direction dir() const { return d_; }
  };
  class Footprint {
    using Array = std::array<bool, Topo::c_boardSize>;

    Array fp_ = {false};

  public:
    Footprint() {}

    bool& operator[](const Position& p) { return fp_[Topo::index(p)]; }
    bool operator[](const Position& p) const { return fp_[Topo::index(p)]; }

    friend std::vector<Change> compare(const Footprint& before, const Footprint& after) {
      std::vector<Change> cs;
      for (auto p : Topo()) {
        switch (after[p] - before[p]) {
          case +1: cs.push_back(Change(p, Direction::placed));  break;
          case -1: cs.push_back(Change(p, Direction::removed)); break;
        }
      }
      return cs;
    }

    Array::const_iterator begin() { return fp_.begin(); }
    Array::const_iterator end()   { return fp_.end(); }
  };

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

    static const uint8_t c_axisSquares = Topo::c_axisSquares;
    static const uint8_t c_boardSize = Topo::c_boardSize;

  public:
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

}

#endif
