#include "dial.h"

using namespace echess;

Dial::Dial() {
  pinMode(gpioRotButton, INPUT_PULLUP);
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachSingleEdge(gpioRotA, gpioRotB);
  encoder.setCount(0);
}

int32_t Dial::count() {
  return encoder.getCount();
}
