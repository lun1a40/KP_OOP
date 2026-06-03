#pragma once

#include <string>

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "exceptions/DomainException.h"

namespace api {

// Отправка JSON-ответа с корректной кодировкой.
inline void sendJson(httplib::Response& res, const nlohmann::json& body, int status = 200) {
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

// Разбор тела запроса как JSON-объекта. Невалидный JSON → ValidationException (400).
inline nlohmann::json parseBody(const httplib::Request& req) {
    try {
        return nlohmann::json::parse(req.body);
    } catch (const std::exception&) {
        throw domain::ValidationException("Тело запроса не является корректным JSON");
    }
}

// Извлечение числового path-параметра из регулярного маршрута.
inline int pathInt(const httplib::Request& req, size_t index) {
    try {
        return std::stoi(req.matches[index].str());
    } catch (const std::exception&) {
        throw domain::ValidationException("Некорректный идентификатор в пути");
    }
}

}  // namespace api
