#include "dial.h"

using namespace echess;

volatile bool Dial::awakenByInterrupt = false;

void Dial::interruptCallback() {
  awakenByInterrupt = true;
}

void Dial::clearInterrupt() {
  awakenByInterrupt = false;
}

Dial::Dial() : max_(0), position_(0) {
  pinMode(c_gpioRotButton, INPUT_PULLUP);
  awakenByInterrupt = false;
  attachInterrupt(c_gpioRotButton, interruptCallback, FALLING);

  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder_.attachSingleEdge(c_gpioRotA, c_gpioRotB);
  encoder_.setCount(0);
}

int32_t Dial::count() {
  return encoder_.getCount();
}

bool Dial::button() {
  unsigned long lastChange = micros();
  int lastValue = digitalRead(c_gpioRotButton);
  while (micros() - lastChange < c_bouncingClearance) {
    int v = digitalRead(c_gpioRotButton);
    if (v != lastValue) {
      lastChange = micros();
      lastValue = v;
    }
  }
  return lastValue == 0;
}

void Dial::init(const int max) {
  max_ = max;
  position_ = 0;
}

bool Dial::wait(const unsigned long timeout) {
  int64_t origin = count();
  int turn = 0;
  bool press = false;
  unsigned long start = millis();

  while (!awakenByInterrupt &&
         (turn = count() - origin) == 0 &&
         (millis() - start < timeout)) {
    delay(10);
  }

  if (awakenByInterrupt) {
    press = button();
    while (press && button()) { delay(10); }
    clearInterrupt();
  }

  if      (turn > 0) position_ = min(position_ + 1, max_);
  else if (turn < 0) position_ = max(position_ - 1, 0);

  return press;
}
