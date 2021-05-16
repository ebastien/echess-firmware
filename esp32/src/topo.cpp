#include "topo.h"

using namespace echess;

void UCIMoves::parse(const char* m) {
  m_.clear();
  for (const char* p = m; *p != '\0';) {
    if (*p == ' ') { ++p; continue; }
    m_.push_back(UCIMove(p));
    p += 4;
  }
}

std::string UCIMoves::str() const {
  std::string s;
  for (auto it = begin(); it != end(); ++it) {
    if (it != begin()) s += " ";
    s += it->c_str();
  }
  return s;
}