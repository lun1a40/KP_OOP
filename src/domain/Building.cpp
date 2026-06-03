#include "domain/Building.h"

#include "exceptions/DomainException.h"

namespace domain {

Building::Building(int id, std::string inventoryNo, std::string name,
                   util::Money initialCost, util::Date acquiredOn,
                   int usefulLifeMonths, int departmentId,
                   double area, std::string address)
    : FixedAsset(id, std::move(inventoryNo), std::move(name), initialCost, acquiredOn,
                 usefulLifeMonths, departmentId),
      area_(area),
      address_(std::move(address)) {
    if (area_ < 0.0) {
        throw ValidationException("Площадь не может быть отрицательной");
    }
}

// Линейная амортизация: износ = initialCost * (monthsElapsed / usefulLifeMonths).
util::Money Building::calculateDepreciation(util::Date onDate) const {
    int monthsElapsed = acquiredOn_.monthsBetween(onDate);
    if (monthsElapsed <= 0) {
        return util::Money(0);
    }
    return depreciationFromRatio(static_cast<double>(monthsElapsed) /
                                 static_cast<double>(usefulLifeMonths_));
}

nlohmann::json Building::toJson() const {
    nlohmann::json j = FixedAsset::toJson();
    j["area"]    = area_;
    j["address"] = address_;
    return j;
}

}  // namespace domain
