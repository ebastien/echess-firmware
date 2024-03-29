#ifndef _ECHESS_DISPLAY_H
#define _ECHESS_DISPLAY_H

#include <U8g2lib.h>
#include "footprint.h"
#include "board.h"

namespace echess {

  class Display {
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

    Display();

  public:
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    static Display& getInstance() {
      static Display instance;
      return instance;
    }

    void prepare();
    void draw();

    void print(const char *msg);
    void print(const std::string& s);
    void print(const int32_t n);

    void print(const Footprint& board);
    void print(const Board& board);
    void print(const UCIMove& move);
    void print(const UCIMove::Promotion p);
  };

}

#endif
