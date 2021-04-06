#include "pointer.h"

using namespace echess;

Pointer::Pointer() {
  s_mcp.begin(c_mcpAddress);
  for (uint8_t p = 0; p < c_mcpGPIOs; p++) {
    s_mcp.pinMode(p, OUTPUT);
  }
  s_mcp.writeGPIOAB(0);
}
