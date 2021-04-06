#ifndef _ECHESS_TOPO_H
#define _ECHESS_TOPO_H

#include <stdint.h>
#include <iterator>

namespace echess {

class Position {
  uint8_t x_, y_;

public:
  Position(uint8_t x, uint8_t y) : x_(x), y_(y) {}

  uint8_t x() const { return x_; }
  uint8_t y() const { return y_; }
};

struct Topo {
  static const uint8_t c_axisSquares = 8;
  static const uint8_t c_boardSize = c_axisSquares * c_axisSquares;

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = uint8_t;
    using pointer           = uint8_t;
    using reference         = Position;
    using value_type        = Position;

    Iterator(pointer p) : p_(p) {}

    reference operator*() const { return position(p_); }
    Iterator& operator++() { p_++; return *this; }  
    Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
    friend bool operator==(const Iterator& a, const Iterator& b) { return a.p_ == b.p_; }
    friend bool operator!=(const Iterator& a, const Iterator& b) { return a.p_ != b.p_; } 

  private:
    pointer p_;
  };

  Topo() {}

  Iterator begin() { return Iterator(0); }
  Iterator end()   { return Iterator(c_boardSize); }

  static uint8_t index(const Position& p) { return p.y() * c_axisSquares + p.x(); }
  static Position position(uint8_t idx) { return Position(idx % c_axisSquares, idx / c_axisSquares); }
};

}

#endif
