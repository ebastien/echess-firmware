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

  scanner.waitForInterrupt();
  scanner.debounce(lastFootprint);
  scanner.clearInterrupt();

  // TODO: process the move

  display.prepare();
  display.print(dial.count());
  display.print(lastFootprint);
  display.draw();

  buzzer.beep();
}
