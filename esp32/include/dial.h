#ifndef _ECHESS_DIAL_H
#define _ECHESS_DIAL_H

#include <ESP32Encoder.h>

class Dial {
  static const uint8_t gpioRotButton = 27;
  static const uint8_t gpioRotA = 25;
  static const uint8_t gpioRotB = 26;

  ESP32Encoder encoder;

  Dial();
 
public:
  Dial(const Dial&) = delete;
  Dial& operator=(const Dial&) = delete;

  static Dial& getInstance() {
    static Dial instance;
    return instance;
  }

  int32_t count();
};

#endif
