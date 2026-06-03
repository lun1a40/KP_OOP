#pragma once

#include <memory>
#include <vector>

#include "domain/FixedAsset.h"
#include "domain/Department.h"
#include "domain/AssetMovement.h"
#include "domain/MaintenanceRecord.h"
#include "nlohmann/json.hpp"

namespace repository {

// Конвертация JSON ↔ доменные объекты.
//
// Сериализация (toJson) живёт в самих доменных классах — это полиморфно.
// Десериализация собрана здесь: assetFromJson — фабричный метод, который
// по полю "category" создаёт нужный наследник FixedAsset. Это единственное
// место, расширяемое при добавлении нового типа фонда.
class JsonMapper {
public:
    // Фабрика: создаёт конкретный наследник FixedAsset по полю "category".
    static std::unique_ptr<domain::FixedAsset> assetFromJson(const nlohmann::json& j);

    static domain::Department       departmentFromJson(const nlohmann::json& j);
    static domain::AssetMovement    movementFromJson(const nlohmann::json& j);
    static domain::MaintenanceRecord maintenanceFromJson(const nlohmann::json& j);

    // Шаблон: единообразная сериализация коллекции любых сущностей с .toJson().
    template <typename T>
    static nlohmann::json toJsonArray(const std::vector<T>& items) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& item : items) {
            arr.push_back(item.toJson());
        }
        return arr;
    }

    // Перегрузка шаблона для контейнеров указателей (полиморфные фонды).
    template <typename T>
    static nlohmann::json toJsonArray(const std::vector<T*>& items) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto* item : items) {
            arr.push_back(item->toJson());
        }
        return arr;
    }
};

}  // namespace repository
