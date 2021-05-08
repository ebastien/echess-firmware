#ifndef _ECHESS_REMOTE_H
#define _ECHESS_REMOTE_H

#include <Arduino.h>

#include <string>

namespace echess {

  class Remote {

    Remote() {};

  public:
    Remote(const Remote&) = delete;
    Remote& operator=(const Remote&) = delete;

    static Remote& getInstance() {
      static Remote instance;
      return instance;
    }

    void connect(const char* ssid, const char* pass);
  };

}

#endif
