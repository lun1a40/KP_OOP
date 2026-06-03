#include "server/HttpServer.h"

#include <iostream>

#include "nlohmann/json.hpp"
#include "exceptions/DomainException.h"

namespace server {

HttpServer::HttpServer(std::string webDir) : webDir_(std::move(webDir)) {
    setupErrorHandling();
    if (!srv_.set_mount_point("/", webDir_)) {
        std::cerr << "WARN: каталог '" << webDir_ << "' не найден — статика недоступна\n";
    }
}

void HttpServer::setupErrorHandling() {
    // Единая точка перевода доменных исключений в HTTP-коды.
    srv_.set_exception_handler(
        [](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
            int status = 500;
            std::string error = "InternalError";
            std::string details = "Внутренняя ошибка сервера";
            try {
                std::rethrow_exception(ep);
            } catch (const domain::NotFoundException& e) {
                status = 404; error = "NotFound";        details = e.what();
            } catch (const domain::ValidationException& e) {
                status = 400; error = "ValidationError";  details = e.what();
            } catch (const domain::InvalidStateException& e) {
                status = 409; error = "InvalidState";     details = e.what();
            } catch (const domain::DomainException& e) {
                status = 400; error = "DomainError";       details = e.what();
            } catch (const std::exception& e) {
                status = 500; error = "InternalError";      details = e.what();
            }
            const nlohmann::json body = {{"error", error}, {"details", details}};
            res.status = status;
            res.set_content(body.dump(), "application/json; charset=utf-8");
        });
}

bool HttpServer::listen(const std::string& host, int port) {
    std::cout << "Server listening on http://localhost:" << port << "\n";
    return srv_.listen(host, port);
}

}  // namespace server
