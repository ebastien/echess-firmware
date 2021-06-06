#include "buzzer.h"

using namespace echess;

Buzzer::Buzzer() {
  pinMode(gpioBuzzer, OUTPUT);
  ledcSetup(chanBuzzer, 440, 8);
}

void Buzzer::tune(double freq, uint32_t duration) const {
  ledcWriteTone(chanBuzzer, freq);
  ledcAttachPin(gpioBuzzer, chanBuzzer);
  delay(duration);
  ledcDetachPin(gpioBuzzer);
}

void Buzzer::beep() const {
  tune(440, 70);
  tune(587, 30);
}

void Buzzer::error() const {
  tune(440, 30);
  tune(294, 70);
}