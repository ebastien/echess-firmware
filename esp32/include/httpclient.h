#ifndef _ECHESS_HTTPCLIENT_H
#define _ECHESS_HTTPCLIENT_H

#include <Arduino.h>
#include "esp_http_client.h"

#include <string>

namespace echess {

  class HTTPClient {
    esp_http_client_config_t config_ = {};
    esp_http_client_handle_t handle_ = nullptr;
    bool connected_ = false;
    std::string baseUrl_, url_, auth_;

    static esp_err_t eventHandler(esp_http_client_event_t *evt);

    int flush();

  public:
    HTTPClient(const std::string& url);
    ~HTTPClient();

    int open(const std::string& url, const std::string& token);
    int post(const std::string& url, const std::string& token);
    void close();
    int read();
    size_t readBytes(char* buffer, size_t length);
    bool isConnected() const { return connected_; }
  };
}

#endif
