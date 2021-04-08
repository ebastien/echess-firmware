#ifndef _ECHESS_DISPLAY_H
#define _ECHESS_DISPLAY_H

#include <U8g2lib.h>
#include "footprint.h"

namespace echess {

  class Display {
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
    uint8_t row;

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
    void print(const int32_t n);
    void print(const Footprint& board);
  };

}

#endif
