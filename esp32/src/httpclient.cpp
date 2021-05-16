
#include "httpclient.h"

using namespace echess;

HTTPClient::HTTPClient(const std::string& url) : baseUrl_(url) {
  config_.url = baseUrl_.c_str();
  config_.user_data = this;
  config_.event_handler = eventHandler;
  handle_ = esp_http_client_init(&config_);
}

HTTPClient::~HTTPClient() { esp_http_client_cleanup(handle_); }

esp_err_t HTTPClient::eventHandler(esp_http_client_event_t *evt) {
  switch (evt->event_id) {
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println("evt:connected");
      ((HTTPClient*)(evt->user_data))->connected_ = true;
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println("evt:finish");
      ((HTTPClient*)(evt->user_data))->connected_ = false;
      break;
    case HTTP_EVENT_ERROR:
      Serial.println("evt:error");
      ((HTTPClient*)(evt->user_data))->connected_ = false;
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println("evt:disconnected");
      ((HTTPClient*)(evt->user_data))->connected_ = false;
      break;
    default: break;
  }
  return ESP_OK;
}

int HTTPClient::open(const std::string& url, const std::string& token) {
  if (isConnected()) {
    close();
  }
  url_ = url;
  auth_ = std::string("Bearer ") + token;
  esp_http_client_set_method(handle_, HTTP_METHOD_GET);
  esp_http_client_set_url(handle_, url_.c_str());
  esp_http_client_set_header(handle_, "Authorization", auth_.c_str());
  esp_err_t err;
  if ((err = esp_http_client_open(handle_, 0)) != ESP_OK) {
      Serial.printf("Failed to open HTTP connection: %s\n", esp_err_to_name(err));
      return -1;
  }
  return esp_http_client_fetch_headers(handle_);
}

int HTTPClient::post(const std::string& url, const std::string& token) {
  if (isConnected()) {
    close();
  }
  url_ = url;
  auth_ = std::string("Bearer ") + token;
  esp_http_client_set_method(handle_, HTTP_METHOD_POST);
  esp_http_client_set_url(handle_, url_.c_str());
  esp_http_client_set_header(handle_, "Authorization", auth_.c_str());
  esp_err_t err;
  if ((err = esp_http_client_open(handle_, 0)) != ESP_OK) {
    Serial.printf("Failed to perform HTTP POST: %s\n", esp_err_to_name(err));
    return -1;
  }
  if (esp_http_client_fetch_headers(handle_) < 0) {
    Serial.println("Failed to fetch HTTP headers");
    return -1;
  }
  return flush();
}

int HTTPClient::flush() {
  char buffer[256];
  while (!esp_http_client_is_complete_data_received(handle_)) {
    int len = esp_http_client_read(handle_, buffer, sizeof(buffer)-1);
    if (len < 0) {
      Serial.println("Failed to flush response");
      return -1;
    }
  }
  return 0;
}

void HTTPClient::close() {
  esp_http_client_close(handle_);
}

int HTTPClient::read() {
  if (!isConnected()) {
    return -1;
  }
  char c;
  if (esp_http_client_read(handle_, &c, 1) <= 0) {
    return -1;
  }
  return c;
}

size_t HTTPClient::readBytes(char* buffer, size_t length) {
  if (!isConnected()) {
    return -1;
  }
  return esp_http_client_read(handle_, buffer, length);
}
