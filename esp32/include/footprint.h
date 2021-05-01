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
    Square p_;
    Direction d_;

  public:
    Change(const Square& p, const Direction d): p_(p), d_(d) {}

    const Square& pos() const { return p_; }
    const Direction dir() const { return d_; }
  };
  class Footprint {
    uint64_t fp_ = 0;

  public:
    Footprint() {}

    bool mark(const Square& p, bool s) {
      bool was = at(p);
      uint8_t m = p.natural();
      fp_ = (fp_ & ~(uint64_t(1) << m)) | (uint64_t(s) << m);
      return was;
    }

    bool at(const Square& p) const { return fp_ & (uint64_t(1) << p.natural()); }
    bool operator[](const Square& p) const { return at(p); }

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

    struct Iterator {
      using iterator_category = std::forward_iterator_tag;
      using value_type        = bool;
      using reference         = bool;

      Iterator(const Footprint& fp, Topo::Iterator it) : fp_(fp), it_(it) {}

      reference operator*() const { return fp_[*it_]; }
      Iterator& operator++() { ++it_; return *this; }
      Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
      friend bool operator==(const Iterator& a, const Iterator& b) { return a.it_ == b.it_; }
      friend bool operator!=(const Iterator& a, const Iterator& b) { return a.it_ != b.it_; }

    private:
      const Footprint& fp_;
      Topo::Iterator it_;
    };

    Iterator begin() { return Iterator(*this, Topo().begin()); }
    Iterator end()   { return Iterator(*this, Topo().end()); }
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
