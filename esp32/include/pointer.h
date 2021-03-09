#ifndef _ECHESS_POINTER_H
#define _ECHESS_POINTER_H

#include <Adafruit_MCP23017.h>

class Pointer {
  static const uint8_t c_mcpAddress = 4;
  static const uint8_t c_mcpGPIOs = 16;

  Adafruit_MCP23017 s_mcp;

  Pointer();

public:
  Pointer(const Pointer&) = delete;
  Pointer& operator=(const Pointer&) = delete;

  static Pointer& getInstance() {
    static Pointer instance;
    return instance;
  }
};

#endif
