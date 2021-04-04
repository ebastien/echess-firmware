#ifndef _ECHESS_BUZZER_H
#define _ECHESS_BUZZER_H

#include <Arduino.h>

class Buzzer {
  static const uint8_t gpioBuzzer = 32;
  static const uint8_t chanBuzzer = 0;

  Buzzer();

public:
  Buzzer(const Buzzer&) = delete;
  Buzzer& operator=(const Buzzer&) = delete;

  static Buzzer& getInstance() {
    static Buzzer instance;
    return instance;
  }

  void beep() const;
};

#endif
