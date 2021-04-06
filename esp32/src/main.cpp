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
  Footprint fp_;
  Machine m_;

  Display& display_;
  Scanner& scanner_;
  Pointer& pointer_;
  Dial& dial_;
  Buzzer& buzzer_;

  Main();

public:
  Main(const Main&) = delete;
  Main& operator=(const Main&) = delete;

  static Main& getInstance() {
    static Main instance;
    return instance;
  }

  Footprint& footprint() { return fp_; }
  Machine& machine() { return m_; }

  void setup();
  void loop();
};

Main::Main() :
  m_(Board()),
  display_(Display::getInstance()),
  scanner_(Scanner::getInstance()),
  pointer_(Pointer::getInstance()),
  dial_(Dial::getInstance()),
  buzzer_(Buzzer::getInstance()) {}

void Main::setup() {
  Serial.begin(9600);

  scanner_.read(fp_);
  display_.prepare();
  display_.print(m_.explain());
  display_.print(fp_);
  display_.draw();
  buzzer_.beep();
}

void Main::loop() {
  Footprint fp(fp_);

  scanner_.waitForInterrupt();
  scanner_.debounce(fp);
  scanner_.clearInterrupt();

  display_.prepare();

  auto ds = compare(fp_, fp);

  m_.transition(ds);

  for (auto i = ds.begin(); i != ds.end(); ++i) {
    char msg[32];
    snprintf(msg, sizeof(msg), "%d %d %s", i->x(), i->y(), i->dir() == Direction::placed ? "P" : "R");
    display_.print(msg);
  }

  fp_ = fp;

  display_.print(m_.explain());
  display_.print(fp_);
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
