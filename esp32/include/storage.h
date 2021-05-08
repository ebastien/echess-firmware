#ifndef _ECHESS_STORAGE_H
#define _ECHESS_STORAGE_H

#include <string>

namespace echess {

  class Storage {

    std::string ssid_, password_, lichessKey_;

    Storage() {}

  public:
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    static Storage& getInstance() {
      static Storage instance;
      return instance;
    }

    bool initialize();

    const char* ssid() const { return ssid_.c_str(); }
    const char* password() const { return password_.c_str(); }
    const char* lichessKey() const { return lichessKey_.c_str(); }
  };

}
#endif
