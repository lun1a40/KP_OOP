#include "domain/FixedAsset.h"

#include <algorithm>

#include "exceptions/DomainException.h"

namespace domain {

FixedAsset::FixedAsset(int id, std::string inventoryNo, std::string name,
                       util::Money initialCost, util::Date acquiredOn,
                       int usefulLifeMonths, int departmentId)
    : id_(id),
      inventoryNo_(std::move(inventoryNo)),
      name_(std::move(name)),
      initialCost_(initialCost),
      acquiredOn_(acquiredOn),
      usefulLifeMonths_(usefulLifeMonths),
      status_(AssetStatus::InUse),
      departmentId_(departmentId) {
    if (inventoryNo_.empty()) {
        throw ValidationException("Инвентарный номер не может быть пустым");
    }
    if (name_.empty()) {
        throw ValidationException("Наименование не может быть пустым");
    }
    if (usefulLifeMonths_ <= 0) {
        throw ValidationException("Срок полезного использования должен быть положительным");
    }
    if (initialCost_.kopecks() < 0) {
        throw ValidationException("Первоначальная стоимость не может быть отрицательной");
    }
    if (departmentId_ <= 0) {
        throw ValidationException("Не указано подразделение");
    }
}

util::Money FixedAsset::depreciationFromRatio(double ratio) const {
    ratio = std::clamp(ratio, 0.0, 1.0);
    return initialCost_ * ratio;
}

void FixedAsset::setStatus(AssetStatus s) {
    status_ = s;
}

void FixedAsset::setDepartment(int newDepartmentId) {
    if (newDepartmentId <= 0) {
        throw ValidationException("Некорректное подразделение");
    }
    departmentId_ = newDepartmentId;
}

void FixedAsset::addMaintenance(MaintenanceRecord record) {
    maintenance_.push_back(std::move(record));
}

util::Money FixedAsset::residualValue(util::Date onDate) const {
    util::Money residual = initialCost_ - calculateDepreciation(onDate);
    if (residual.kopecks() < 0) {
        return util::Money(0);
    }
    return residual;
}

nlohmann::json FixedAsset::toJson() const {
    nlohmann::json maintenanceArr = nlohmann::json::array();
    for (const auto& r : maintenance_) {
        maintenanceArr.push_back(r.toJson());
    }
    return {
        {"id",               id_},
        {"inventoryNo",      inventoryNo_},
        {"name",             name_},
        {"category",         category()},          // полиморфный вызов
        {"initialCost",      initialCost_.kopecks()},
        {"acquiredOn",       acquiredOn_.toIso()},
        {"usefulLifeMonths", usefulLifeMonths_},
        {"status",           toString(status_)},
        {"departmentId",     departmentId_},
        {"maintenance",      maintenanceArr},
    };
}

bool FixedAsset::operator==(const FixedAsset& o) const {
    return inventoryNo_ == o.inventoryNo_;
}

bool FixedAsset::operator<(const FixedAsset& o) const {
    return id_ < o.id_;
}

std::ostream& operator<<(std::ostream& os, const FixedAsset& a) {
    return os << "[" << a.category() << " #" << a.id_ << " " << a.inventoryNo_
              << " \"" << a.name_ << "\"]";
}

}  // namespace domain
