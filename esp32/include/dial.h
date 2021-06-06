#ifndef _ECHESS_DIAL_H
#define _ECHESS_DIAL_H

#include <ESP32Encoder.h>

namespace echess {

  class Dial {
    static const uint8_t c_gpioRotButton = 27;
    static const uint8_t c_gpioRotA = 25;
    static const uint8_t c_gpioRotB = 26;
    static const unsigned long c_bouncingClearance = 5000; // 5ms

    static volatile bool awakenByInterrupt;
    static void interruptCallback();

    ESP32Encoder encoder_;
    int max_;
    int position_;

    Dial();

    void clearInterrupt();
    int32_t count();
    bool button();

  public:
    Dial(const Dial&) = delete;
    Dial& operator=(const Dial&) = delete;

    static Dial& getInstance() {
      static Dial instance;
      return instance;
    }

    void init(const int max);
    bool wait(const unsigned long timeout);
    int position() const { return position_; }
  };

}

#endif
