#include "footprint.h"

volatile bool Scanner::awakenByInterrupt = false;

void Scanner::interruptCallback() {
  awakenByInterrupt = true;
}

Scanner::Scanner() {
  pinMode(c_gpioInterrupt, INPUT);
  for (uint8_t n = 0; n < c_mcpSquaresUnits; n++) {
    s_mcps[n].begin(n);
    s_mcps[n].setupInterrupts(true, false, HIGH);
    for (uint8_t p = 0; p < c_mcpGPIOs; p++) {
      s_mcps[n].pinMode(p, INPUT);
      s_mcps[n].pullUp(p, HIGH);
      s_mcps[n].setupInterruptPin(p, CHANGE);
    }
  }
  awakenByInterrupt = false;
  attachInterrupt(c_gpioInterrupt, interruptCallback, RISING);
}

void Scanner::waitForInterrupt() {
  for (uint8_t n = 0; n < c_mcpSquaresUnits; n++) {
    s_mcps[n].getLastInterruptPinValue();
  }
  while (!awakenByInterrupt) { delay(10); }
}

void Scanner::clearInterrupt() {
  awakenByInterrupt = false;
}

bool Scanner::read(Footprint& f) {
  bool hasChanged = false;
  for (uint8_t n = 0; n < c_mcpSquaresUnits; n++) {
    const uint16_t gpios = s_mcps[n].readGPIOAB();
    for (uint8_t p = 0; p < c_mcpGPIOs; p++) {
      const uint8_t idx = n * c_mcpGPIOs + p;
      bool square = ! (gpios & (1 << p));
      if (f.fp_[idx] != square) {
        f.fp_[idx] = square;
        hasChanged = true;
      }
    }
  }
  return hasChanged;
}

void Scanner::debounce(Footprint& f) {
  unsigned long lastChange = micros();
  while (micros() - lastChange < c_bouncingClearance) {
    if (read(f)) {
      lastChange = micros();
    }
  }
}

Footprint::Footprint() {
  for (uint8_t n = 0; n < c_boardSize; n++) {
    fp_[n] = false;
  }
}
