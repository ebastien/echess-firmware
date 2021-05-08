#ifndef _ECHESS_REMOTE_H
#define _ECHESS_REMOTE_H

#include <Arduino.h>

#include <string>

namespace echess {

  class Remote {

    std::string ssid_, pass_;

    Remote() {};

  public:
    Remote(const Remote&) = delete;
    Remote& operator=(const Remote&) = delete;

    static Remote& getInstance() {
      static Remote instance;
      return instance;
    }

    void configure(const std::string& ssid, const std::string& pass) {
      ssid_ = ssid; pass_ = pass;
    }

    void connect();
  };

}

#endif
