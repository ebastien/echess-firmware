#include "topo.h"

#include <cstring>

using namespace echess;

void UCIMoves::parse(const char* moves) {
  m_.clear();
  if (moves != nullptr) {
    for (const char* p = moves; *p != '\0';) {
      if (*p == ' ') { ++p; continue; }
      const UCIMove m(p);
      m_.push_back(m);
      p += strlen(m.c_str());
    }
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

UCIState::UCIState(const char* fen, const UCIMoves& moves) : m_(moves) {
  fen_ = fen != nullptr ? fen : "";
}
