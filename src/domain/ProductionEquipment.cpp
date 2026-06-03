#include "domain/ProductionEquipment.h"

#include "exceptions/DomainException.h"

namespace domain {

ProductionEquipment::ProductionEquipment(int id, std::string inventoryNo, std::string name,
                                         util::Money initialCost, util::Date acquiredOn,
                                         int usefulLifeMonths, int departmentId,
                                         std::string model, int capacityPerHour, int runHours,
                                         ToyCategory toyCategory, AgeGroup ageGroup)
    : FixedAsset(id, std::move(inventoryNo), std::move(name), initialCost, acquiredOn,
                 usefulLifeMonths, departmentId),
      model_(std::move(model)),
      capacityPerHour_(capacityPerHour),
      runHours_(runHours),
      toyCategory_(toyCategory),
      ageGroup_(ageGroup) {
    if (capacityPerHour_ < 0) {
        throw ValidationException("Производительность не может быть отрицательной");
    }
    if (runHours_ < 0) {
        throw ValidationException("Наработка часов не может быть отрицательной");
    }
}

// Амортизация по наработке: износ = initialCost * (runHours / expectedTotalHours).
util::Money ProductionEquipment::calculateDepreciation(util::Date /*onDate*/) const {
    const double expectedTotalHours =
        static_cast<double>(usefulLifeMonths_) * 22.0 * 8.0;  // 22 дня × 8 часов
    if (expectedTotalHours <= 0.0) {
        return util::Money(0);
    }
    return depreciationFromRatio(static_cast<double>(runHours_) / expectedTotalHours);
}

nlohmann::json ProductionEquipment::toJson() const {
    nlohmann::json j = FixedAsset::toJson();   // общие поля
    j["model"]           = model_;
    j["capacityPerHour"] = capacityPerHour_;
    j["runHours"]        = runHours_;
    j["toyCategory"]     = toString(toyCategory_);
    j["ageGroup"]        = toString(ageGroup_);
    return j;
}

void ProductionEquipment::incrementRunHours(int hours) {
    if (hours < 0) {
        throw ValidationException("Прирост наработки не может быть отрицательным");
    }
    runHours_ += hours;
}

}  // namespace domain
