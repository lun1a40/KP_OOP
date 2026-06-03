#include "service/AssetService.h"

#include <algorithm>
#include <cctype>

#include "domain/MaintenanceRecord.h"
#include "exceptions/DomainException.h"

namespace service {

using namespace domain;

namespace {

// Регистронезависимый поиск подстроки (только для ASCII-«опускания»;
// кириллица сравнивается как есть — достаточно для учебной фильтрации).
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool contains(const std::string& haystack, const std::string& needle) {
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}

}  // namespace

AssetService::AssetService(repository::IAssetRepository& assets,
                           repository::IDepartmentRepository& departments,
                           MovementService& movements)
    : assets_(assets), departments_(departments), movements_(movements) {}

FixedAsset& AssetService::require(int id) {
    FixedAsset* a = assets_.find(id);
    if (!a) throw NotFoundException("Asset", id);
    return *a;
}

const FixedAsset& AssetService::require(int id) const {
    const FixedAsset* a = assets_.find(id);
    if (!a) throw NotFoundException("Asset", id);
    return *a;
}

void AssetService::ensureDepartmentExists(int departmentId) const {
    if (!departments_.find(departmentId)) {
        throw ValidationException("Подразделение не найдено: id=" + std::to_string(departmentId));
    }
}

void AssetService::ensureUniqueInventoryNo(const std::string& inventoryNo, int excludeId) const {
    for (const auto* a : const_cast<repository::IAssetRepository&>(assets_).all()) {
        if (a->id() != excludeId && a->inventoryNo() == inventoryNo) {
            throw ValidationException("Инвентарный номер уже используется: " + inventoryNo);
        }
    }
}

int AssetService::add(std::unique_ptr<FixedAsset> asset) {
    ensureDepartmentExists(asset->departmentId());
    ensureUniqueInventoryNo(asset->inventoryNo(), /*excludeId=*/0);

    const int deptId = asset->departmentId();
    const int id = assets_.add(std::move(asset));
    movements_.recordAcquisition(id, deptId);
    return id;
}

void AssetService::update(int id, std::unique_ptr<FixedAsset> newData) {
    const FixedAsset& existing = require(id);

    if (existing.category() != newData->category()) {
        throw ValidationException("Нельзя менять категорию существующего фонда");
    }
    ensureUniqueInventoryNo(newData->inventoryNo(), /*excludeId=*/id);
    ensureDepartmentExists(newData->departmentId());

    // Сохраняем то, что не редактируется через форму: статус и историю ТО.
    newData->setId(id);
    newData->setStatus(existing.status());
    for (const auto& record : existing.maintenance()) {
        newData->addMaintenance(record);
    }

    assets_.remove(id);
    assets_.add(std::move(newData));
}

void AssetService::remove(int id) {
    const FixedAsset& asset = require(id);
    if (asset.status() != AssetStatus::WrittenOff) {
        throw InvalidStateException("Удалить можно только списанный фонд");
    }
    assets_.remove(id);
}

void AssetService::transfer(int id, int newDepartmentId, const std::string& note) {
    FixedAsset& asset = require(id);
    if (asset.status() == AssetStatus::WrittenOff) {
        throw InvalidStateException("Списанный фонд нельзя переместить");
    }
    ensureDepartmentExists(newDepartmentId);

    const int from = asset.departmentId();
    if (from == newDepartmentId) {
        throw ValidationException("Фонд уже находится в этом подразделении");
    }
    asset.setDepartment(newDepartmentId);
    assets_.persist();
    movements_.recordTransfer(id, from, newDepartmentId, note);
}

void AssetService::sendToRepair(int id, const std::string& note) {
    FixedAsset& asset = require(id);
    if (asset.status() != AssetStatus::InUse) {
        throw InvalidStateException("На ремонт можно отправить только используемый фонд");
    }
    asset.setStatus(AssetStatus::InRepair);
    assets_.persist();
    movements_.recordRepair(id, MovementType::SentToRepair, note);
}

void AssetService::backFromRepair(int id, util::Money repairCost, const std::string& description) {
    FixedAsset& asset = require(id);
    if (asset.status() != AssetStatus::InRepair) {
        throw InvalidStateException("Фонд не находится в ремонте");
    }
    asset.setStatus(AssetStatus::InUse);
    const int maintenanceId = static_cast<int>(asset.maintenance().size()) + 1;
    asset.addMaintenance(MaintenanceRecord(maintenanceId, util::Date::today(),
                                           MaintenanceType::Emergency, repairCost,
                                           description, "admin"));
    assets_.persist();
    movements_.recordRepair(id, MovementType::BackFromRepair, description);
}

void AssetService::writeOff(int id, const std::string& reason) {
    FixedAsset& asset = require(id);
    if (asset.status() == AssetStatus::WrittenOff) {
        throw InvalidStateException("Фонд уже списан");
    }
    asset.setStatus(AssetStatus::WrittenOff);
    assets_.persist();
    movements_.recordWriteOff(id, reason);
}

const FixedAsset& AssetService::get(int id) const {
    return require(id);
}

std::vector<FixedAsset*> AssetService::list(const AssetFilter& filter) {
    std::vector<FixedAsset*> result;
    for (auto* a : assets_.all()) {
        if (filter.category && a->category() != *filter.category) continue;
        if (filter.departmentId && a->departmentId() != *filter.departmentId) continue;
        if (filter.status && a->status() != *filter.status) continue;
        if (filter.query &&
            !contains(a->name(), *filter.query) &&
            !contains(a->inventoryNo(), *filter.query)) {
            continue;
        }
        result.push_back(a);
    }
    return result;
}

}  // namespace service
