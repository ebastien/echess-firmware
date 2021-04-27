#ifndef _ECHESS_TOPO_H
#define _ECHESS_TOPO_H

#include <stdint.h>
#include <iterator>

namespace echess {

  struct Position {
    enum Label : uint8_t {
      a8=0, b8, c8, d8, e8, f8, g8, h8,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a1, b1, c1, d1, e1, f1, g1, h1
    };

    Position(Label l) : l_(l) {}
    Position(uint8_t x, uint8_t y) : l_(Label((7 - y) * 8 + x)) {}
    Position(const char* n) : Position(n[0] - 'a', n[1] - '1') {}

    friend bool operator==(const Position& a, const Position& b) { return a.l_ == b.l_; }
    friend bool operator!=(const Position& a, const Position& b) { return a.l_ != b.l_; }

    uint8_t x() const { return l_ % 8; }
    uint8_t y() const { return 7 - l_ / 8; }
    uint8_t natural() const { return y() * 8 + x(); }
    uint8_t forsyth() const { return l_; }

    void terse(char* t) const { t[0] = 'a' + x(); t[1] = '1' + y(); }

    static Position fromNatural(uint8_t index) { return Position(index % 8, index / 8); }
    static Position fromForsyth(uint8_t index) { return Position(Label(index)); }

  private:
    Label l_;
  };

  struct Topo {
    struct Iterator {
      using iterator_category = std::forward_iterator_tag;
      using difference_type   = uint8_t;
      using pointer           = uint8_t;
      using value_type        = Position;
      using reference         = value_type;

      Iterator(pointer p) : p_(p) {}

      reference operator*() const { return Position::fromNatural(p_); }
      Iterator& operator++() { ++p_; return *this; }
      Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
      friend bool operator==(const Iterator& a, const Iterator& b) { return a.p_ == b.p_; }
      friend bool operator!=(const Iterator& a, const Iterator& b) { return a.p_ != b.p_; }

    private:
      pointer p_;
    };

    Topo() {}

    Iterator begin() { return Iterator(0); }
    Iterator end()   { return Iterator(64); }
  };
}

#endif
