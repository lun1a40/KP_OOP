#pragma once

#include <string>

#include "nlohmann/json.hpp"

namespace repository {

// Чтение/запись JSON-файла хранилища.
// Если файла нет — readArray возвращает пустой массив (репозиторий стартует пустым).
nlohmann::json readArray(const std::string& path);
void           writeJson(const std::string& path, const nlohmann::json& data);

}  // namespace repository
