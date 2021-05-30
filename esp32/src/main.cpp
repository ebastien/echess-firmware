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
    buzzer_.beep();
    splash();

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
    const auto state = lichess_.waitGameState();
    if (!state || state->initial().empty()) {
      Serial.println("fatal: cannot read game state");
      while (true) { delay(100); }
    }
    Board board(state->initial());
    if (!board.fromMoves(state->moves())) {
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

  void Main::splash() {
    display_.prepare();
    display_.print("starting ...");
    display_.draw();
    pointer_.flash();
  }

  void Main::redraw() {

    const auto lastMove = m_.board().lastMove();

    display_.prepare();

    if (lastMove) {
      display_.print(lastMove->uci().c_str());

      if (tick_ % 2 == 0) {
        pointer_.point(lastMove->from());
      } else {
        pointer_.point(lastMove->to());
      }
    } else {
      pointer_.clear();
    }

    display_.print(m_.explain());

    if (tick_ % 4 == 0) {
      display_.print(m_.footprint());
    } else {
      display_.print(m_.board());
    }

    display_.draw();
  }

  void Main::loop() {

    const int nbMoves(m_.board().length());

    if (m_.isReady() && m_.board().player() != lichess_.player()) {

      const auto state = lichess_.waitGameState(nbMoves);

      if (!state) {
        Serial.println("warning: cannot read game state");
        return;
      }

      if (!m_.transition(state->moves())) {
        Serial.println("fatal: cannot play moves");
        while (true) { delay(100); }
      }

      buzzer_.beep();
      pointer_.flash();

    } else {

      Footprint fp(m_.footprint());

      if (scanner_.waitForInterrupt(1000)) {
        scanner_.debounce(fp);
        scanner_.clearInterrupt();

        if (!m_.transition(fp)) {
          Serial.println("fatal: cannot play move");
          while (true) { delay(100); }
        }

        buzzer_.beep();
        pointer_.flash();
      }

      if (m_.board().length() > nbMoves) {
        lichess_.makeMove(m_.board().lastMove()->uci());
      }
    }

    redraw();

    Serial.print("Free heap (kB): "); Serial.println(ESP.getFreeHeap() / 1024);

    ++tick_;
  }

}

void setup() {
  echess::Main::getInstance().setup();
}

void loop() {
  echess::Main::getInstance().loop();
}
