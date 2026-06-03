#include "repository/JsonMapper.h"

#include "domain/ProductionEquipment.h"
#include "domain/Building.h"
#include "domain/Vehicle.h"
#include "domain/OfficeEquipment.h"
#include "exceptions/DomainException.h"

namespace repository {

using nlohmann::json;
using namespace domain;

namespace {

// Деньги в JSON хранятся целым числом копеек.
util::Money moneyFrom(const json& j) {
    return util::Money(j.get<std::int64_t>());
}

// Общие для всех фондов поля (распарсенные заранее, чтобы передать в конструктор).
struct CommonFields {
    int          id;
    std::string  inventoryNo;
    std::string  name;
    util::Money  initialCost;
    util::Date   acquiredOn;
    int          usefulLifeMonths;
    int          departmentId;
};

CommonFields readCommon(const json& j) {
    return CommonFields{
        j.value("id", 0),
        j.at("inventoryNo").get<std::string>(),
        j.at("name").get<std::string>(),
        moneyFrom(j.at("initialCost")),
        util::Date::fromIso(j.at("acquiredOn").get<std::string>()),
        j.at("usefulLifeMonths").get<int>(),
        j.at("departmentId").get<int>(),
    };
}

}  // namespace

std::unique_ptr<FixedAsset> JsonMapper::assetFromJson(const json& j) {
    try {
        const std::string category = j.at("category").get<std::string>();
        const CommonFields c = readCommon(j);

        std::unique_ptr<FixedAsset> asset;

        // --- Фабрика по category ---
        if (category == "production") {
            asset = std::make_unique<ProductionEquipment>(
                c.id, c.inventoryNo, c.name, c.initialCost, c.acquiredOn,
                c.usefulLifeMonths, c.departmentId,
                j.at("model").get<std::string>(),
                j.at("capacityPerHour").get<int>(),
                j.value("runHours", 0),
                toyCategoryFromString(j.at("toyCategory").get<std::string>()),
                ageGroupFromString(j.at("ageGroup").get<std::string>()));
        } else if (category == "building") {
            asset = std::make_unique<Building>(
                c.id, c.inventoryNo, c.name, c.initialCost, c.acquiredOn,
                c.usefulLifeMonths, c.departmentId,
                j.value("area", 0.0),
                j.value("address", std::string{}));
        } else if (category == "vehicle") {
            asset = std::make_unique<Vehicle>(
                c.id, c.inventoryNo, c.name, c.initialCost, c.acquiredOn,
                c.usefulLifeMonths, c.departmentId,
                j.value("licensePlate", std::string{}),
                j.value("mileage", 0),
                j.at("expectedTotalMileage").get<int>());
        } else if (category == "office") {
            asset = std::make_unique<OfficeEquipment>(
                c.id, c.inventoryNo, c.name, c.initialCost, c.acquiredOn,
                c.usefulLifeMonths, c.departmentId,
                j.value("serialNumber", std::string{}));
        } else {
            throw ValidationException("Неизвестная категория фонда: " + category);
        }

        // Статус (для загружаемых объектов) — у нового фонда по умолчанию InUse.
        if (j.contains("status")) {
            asset->setStatus(assetStatusFromString(j.at("status").get<std::string>()));
        }

        // Журнал ТО (композиция).
        if (j.contains("maintenance") && j.at("maintenance").is_array()) {
            for (const auto& m : j.at("maintenance")) {
                asset->addMaintenance(maintenanceFromJson(m));
            }
        }

        return asset;
    } catch (const ValidationException&) {
        throw;  // уже доменная ошибка — пробрасываем
    } catch (const std::exception& e) {
        // Любая ошибка структуры/парсинга JSON → ошибка валидации (HTTP 400).
        throw ValidationException(std::string("Некорректные данные фонда: ") + e.what());
    }
}

Department JsonMapper::departmentFromJson(const json& j) {
    try {
        return Department(
            j.value("id", 0),
            j.at("name").get<std::string>(),
            j.value("head", std::string{}),
            departmentTypeFromString(j.value("type", std::string{"Production"})));
    } catch (const ValidationException&) {
        throw;
    } catch (const std::exception& e) {
        throw ValidationException(std::string("Некорректные данные подразделения: ") + e.what());
    }
}

AssetMovement JsonMapper::movementFromJson(const json& j) {
    try {
        std::optional<int> from;
        std::optional<int> to;
        if (j.contains("fromDepartmentId") && !j.at("fromDepartmentId").is_null()) {
            from = j.at("fromDepartmentId").get<int>();
        }
        if (j.contains("toDepartmentId") && !j.at("toDepartmentId").is_null()) {
            to = j.at("toDepartmentId").get<int>();
        }
        return AssetMovement(
            j.value("id", 0),
            j.at("assetId").get<int>(),
            movementTypeFromString(j.at("type").get<std::string>()),
            from, to,
            util::Date::fromIso(j.at("date").get<std::string>()),
            j.value("note", std::string{}),
            j.value("performedBy", std::string{}));
    } catch (const std::exception& e) {
        throw ValidationException(std::string("Некорректные данные движения: ") + e.what());
    }
}

MaintenanceRecord JsonMapper::maintenanceFromJson(const json& j) {
    try {
        return MaintenanceRecord(
            j.value("id", 0),
            util::Date::fromIso(j.at("date").get<std::string>()),
            maintenanceTypeFromString(j.value("type", std::string{"Planned"})),
            moneyFrom(j.at("cost")),
            j.value("description", std::string{}),
            j.value("performedBy", std::string{}));
    } catch (const std::exception& e) {
        throw ValidationException(std::string("Некорректные данные ТО: ") + e.what());
    }
}

}  // namespace repository
