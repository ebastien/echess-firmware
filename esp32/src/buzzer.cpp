#include "buzzer.h"

Buzzer::Buzzer() {
  pinMode(gpioBuzzer, OUTPUT);
  ledcSetup(chanBuzzer, 440, 8);
}

void Buzzer::beep() const {
  ledcWriteTone(chanBuzzer, 440);
  ledcAttachPin(gpioBuzzer, chanBuzzer);
  delay(100);
  ledcDetachPin(gpioBuzzer);
}
