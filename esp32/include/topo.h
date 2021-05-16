#ifndef _ECHESS_TOPO_H
#define _ECHESS_TOPO_H

#include <stdint.h>
#include <iterator>
#include <array>
#include <vector>

namespace echess {

  struct Square {
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

    Square(Label l) : l_(l) {}
    Square(uint8_t x, uint8_t y) : l_(Label((7 - y) * 8 + x)) {}
    Square(const char* n) : Square(n[0] - 'a', n[1] - '1') {}

    friend bool operator==(const Square& a, const Square& b) { return a.l_ == b.l_; }
    friend bool operator!=(const Square& a, const Square& b) { return a.l_ != b.l_; }

    uint8_t x() const { return l_ % 8; }
    uint8_t y() const { return 7 - l_ / 8; }
    uint8_t natural() const { return y() * 8 + x(); }
    uint8_t forsyth() const { return l_; }
    char ax() const { return 'a' + x(); }
    char ay() const { return '1' + y(); }

    void terse(char* t) const { t[0] = 'a' + x(); t[1] = '1' + y(); }
    std::string terse() const { char t[3] = {0}; terse(t); return std::string(t); }

    static Square fromNatural(uint8_t index) { return Square(index % 8, index / 8); }
    static Square fromForsyth(uint8_t index) { return Square(Label(index)); }

  private:
    Label l_;
  };

  struct Topo {
    struct Iterator {
      using iterator_category = std::forward_iterator_tag;
      using difference_type   = uint8_t;
      using pointer           = uint8_t;
      using value_type        = Square;
      using reference         = value_type;

      Iterator(pointer p) : p_(p) {}

      reference operator*() const { return Square::fromNatural(p_); }
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

  // struct UCISquare {
  //   UCISquare(const Square& s) : s_({ s.ax(), s.ay(), '\0' }) {}

  //   const char* c_str() const { return s_.data(); }

  // private:
  //   std::array<char, 3> s_;
  // };

  class UCIMove {
    std::array<char, 6> m_;

  public:
    enum Promotion : uint8_t { queen = 0, rook, bishop, knight };

    UCIMove(const char* m) : m_({ m[0], m[1], m[2], m[3], m[4] == ' ' ? '\0' : m[4], '\0' }) {}
    UCIMove(const std::string& m) : UCIMove(m.c_str()) {}
    UCIMove(const Square& from, const Square& to) : m_({ from.ax(), from.ay(), to.ax(), to.ay(), '\0', '\0' }) {}
    UCIMove(const Square& from, const Square& to, const Promotion p);

    Square from() const { return Square(m_.data()); }
    Square to() const { return Square(m_.data() + 2); }
    const char* c_str() const { return m_.data(); }
  };

  class Moves {
    std::vector<UCIMove> m_;

    void parse(const char* m);

  public:
    Moves() {}
    Moves(const char* m) { parse(m); }
    Moves& operator=(const char* m) { parse(m); return *this; }

    void clear() { m_.clear(); }
    Moves& operator+=(const UCIMove& m) { m_.push_back(m); return *this; }

    int length() const { return m_.size(); }
    const UCIMove& last() const { return m_.back(); }
    std::string uci() const;

    std::vector<UCIMove>::const_iterator begin() const { return m_.begin(); }
    std::vector<UCIMove>::const_iterator end()   const { return m_.end(); }
  };
}

#endif
