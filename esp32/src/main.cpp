#include <Arduino.h>

#include "board.h"
#include "footprint.h"
#include "pointer.h"
#include "dial.h"
#include "display.h"
#include "buzzer.h"

Footprint lastFootprint;

void setup() {
  Serial.begin(9600);

  auto& display = Display::getInstance();
  auto& scanner = Scanner::getInstance();
  auto& pointer = Pointer::getInstance();
  auto& dial = Dial::getInstance();
  auto& buzzer = Buzzer::getInstance();

  scanner.read(lastFootprint);
  display.prepare();
  display.print(dial.count());
  display.print(lastFootprint);
  display.draw();
  buzzer.beep();
}

void loop() {
  auto& display = Display::getInstance();
  auto& dial = Dial::getInstance();
  auto& scanner = Scanner::getInstance();
  auto& buzzer = Buzzer::getInstance();

  Footprint fp(lastFootprint);

  scanner.waitForInterrupt();
  scanner.debounce(fp);
  scanner.clearInterrupt();

  display.prepare();

  // TODO: process the move

  auto ds(lastFootprint.compare(fp));
  for (auto i = ds.begin(); i != ds.end(); ++i) {
    char msg[32];
    snprintf(msg, sizeof(msg), "%d %d %s", i->x(), i->y(), i->dir() == Direction::placed ? "P" : "R");
    display.print(msg);
  }

  lastFootprint = fp;

  display.print(dial.count());
  display.print(lastFootprint);
  display.draw();

  buzzer.beep();
}
