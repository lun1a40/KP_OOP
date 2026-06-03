#pragma once

#include <string>

#include "httplib.h"

namespace server {

// Обёртка над httplib::Server: централизованная обработка ошибок,
// раздача статики из web/, регистрация маршрутов контроллерами.
class HttpServer {
public:
    explicit HttpServer(std::string webDir);

    // Доступ к «сырому» серверу для регистрации маршрутов контроллерами.
    httplib::Server& raw() { return srv_; }

    // Блокирующий запуск. Возвращает false, если не удалось занять порт.
    bool listen(const std::string& host, int port);

private:
    void setupErrorHandling();

    httplib::Server srv_;
    std::string     webDir_;
};

}  // namespace server
