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

    void redraw();

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

  void Main::setup() {
    Serial.begin(9600);
    delay(5);

    if (!storage_.initialize()) {
      while (true) { delay(100); }
    }

    remote_.connect(storage_.ssid(), storage_.password());

    lichess_.setToken(storage_.lichessKey());
    lichess_.findGame();
    while (!lichess_.isGamePlaying()) {
      delay(1000);
      lichess_.findGame();
    }
    const auto moves = lichess_.waitGameState();
    if (!moves) {
      Serial.println("fatal: cannot read game state");
      while (true) { delay(100); }
    }
    Board board;
    if (!board.fromMoves(*moves)) {
      Serial.println("fatal: cannot play initial moves");
      while (true) { delay(100); }
    }
    m_.reset(board);

    Footprint fp;
    scanner_.read(fp);
    m_.transition(fp);

    redraw();

    buzzer_.beep();
  }

  void Main::redraw() {

    const auto lastMove = m_.board().lastMove();

    display_.prepare();

    if (lastMove) {
      display_.print(lastMove->uci().c_str());
    }

    display_.print(m_.explain());

    if (tick_ % 2 == 0) {
      display_.print(m_.footprint());
    } else {
      display_.print(m_.board());
    }

    display_.draw();
  }

  void Main::loop() {

    if (m_.isReady() && m_.board().player() != lichess_.player()) {

      const auto lastMove = m_.board().lastMove();
      if (lastMove) {
        lichess_.makeMove(lastMove->uci());
      }

      const auto moves = lichess_.waitGameState(m_.board().length());

      if (!moves) {
        Serial.println("warning: cannot read game state");
        return;
      }

      m_.transition(*moves);
      buzzer_.beep();

    } else {

      Footprint fp(m_.footprint());

      if (scanner_.waitForInterrupt(1000)) {
        scanner_.debounce(fp);
        scanner_.clearInterrupt();
        m_.transition(fp);
        buzzer_.beep();
      }
    }

    redraw();

    ++tick_;
  }

}

void setup() {
  echess::Main::getInstance().setup();
}

void loop() {
  echess::Main::getInstance().loop();
}
