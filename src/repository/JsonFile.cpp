#include "repository/JsonFile.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace repository {

namespace fs = std::filesystem;

nlohmann::json readArray(const std::string& path) {
    if (!fs::exists(path)) {
        return nlohmann::json::array();
    }
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Не удалось открыть файл для чтения: " + path);
    }
    nlohmann::json data;
    try {
        in >> data;
    } catch (const std::exception& e) {
        throw std::runtime_error("Повреждён JSON в файле " + path + ": " + e.what());
    }
    if (data.is_null()) {
        return nlohmann::json::array();
    }
    if (!data.is_array()) {
        throw std::runtime_error("Ожидался JSON-массив в файле " + path);
    }
    return data;
}

void writeJson(const std::string& path, const nlohmann::json& data) {
    const fs::path p(path);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + path);
    }
    out << data.dump(2);
}

}  // namespace repository
