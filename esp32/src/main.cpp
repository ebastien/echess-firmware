#include <Arduino.h>

#include "board.h"
#include "footprint.h"
#include "pointer.h"
#include "dial.h"
#include "display.h"
#include "buzzer.h"
#include "state.h"

namespace echess {

  class Main {
    Display& display_;
    Scanner& scanner_;
    Pointer& pointer_;
    Dial& dial_;
    Buzzer& buzzer_;

    Machine m_;

    Main() :
      display_(Display::getInstance()),
      scanner_(Scanner::getInstance()),
      pointer_(Pointer::getInstance()),
      dial_(Dial::getInstance()),
      buzzer_(Buzzer::getInstance()),
      m_(testBoard()) {}

  public:
    Main(const Main&) = delete;
    Main& operator=(const Main&) = delete;

    static Main& getInstance() {
      static Main instance;
      return instance;
    }

    void setup();
    void loop();

    static Board testBoard() {
      // return Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      return Board("1n1qkb2/1p6/8/8/8/8/P7/R2QK2R w KQkq - 0 1");
    }
  };

  void Main::setup() {
    Serial.begin(9600);

    display_.prepare();
    display_.print(m_.explain());
    display_.print(m_.board());
    display_.draw();

    buzzer_.beep();
  }

  void Main::loop() {
    Footprint fp(m_.footprint());

    scanner_.waitForInterrupt();
    scanner_.debounce(fp);
    scanner_.clearInterrupt();

    m_.transition(fp);

    display_.prepare();
    display_.print(m_.explain());
    display_.print(m_.board());
    display_.draw();

    buzzer_.beep();
  }

}

void setup() {
  echess::Main::getInstance().setup();
}

void loop() {
  echess::Main::getInstance().loop();
}
