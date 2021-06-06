#ifndef _ECHESS_MAIN_H
#define _ECHESS_MAIN_H

#include <Arduino.h>

#include "board.h"
#include "footprint.h"
#include "pointer.h"
#include "dial.h"
#include "display.h"
#include "buzzer.h"
#include "state.h"
#include "remote.h"
#include "lichess.h"
#include "storage.h"

namespace echess {

  class Main {
    Display& display_;
    Scanner& scanner_;
    Pointer& pointer_;
    Dial& dial_;
    Buzzer& buzzer_;
    Remote& remote_;
    Lichess& lichess_;
    Storage& storage_;

    Machine m_;
    unsigned long tick_ = 0;

    Main() :
      display_(Display::getInstance()),
      scanner_(Scanner::getInstance()),
      pointer_(Pointer::getInstance()),
      dial_(Dial::getInstance()),
      buzzer_(Buzzer::getInstance()),
      remote_(Remote::getInstance()),
      lichess_(Lichess::getInstance()),
      storage_(Storage::getInstance()) {}

    void splash();
    void redraw();
    void reset();
    void fatal(const char* msg);
    void notify(bool error = false);

    static const char* showPromotion(UCIMove::Promotion p);
    UCIMove::Promotion askPromotion();

  public:
    Main(const Main&) = delete;
    Main& operator=(const Main&) = delete;

    static Main& getInstance() {
      static Main instance;
      return instance;
    }

    void setup();
    void loop();
  };
}

#endif
