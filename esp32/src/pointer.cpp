#include "pointer.h"

using namespace echess;

Pointer::Pointer() {
  s_mcp.begin(c_mcpAddress);
  for (uint8_t p = 0; p < c_mcpGPIOs; p++) {
    s_mcp.pinMode(p, OUTPUT);
  }
  clear();
}

void Pointer::point(const Square& s) {
  s_mcp.writeGPIOAB((uint16_t)1 << (8 + s.x()) | (uint16_t)1 << s.y());
}

void Pointer::clear() {
  s_mcp.writeGPIOAB(0);
}

void Pointer::flash() {
  s_mcp.writeGPIOAB(0xffff);
  delay(100);
  s_mcp.writeGPIOAB(0);
}
