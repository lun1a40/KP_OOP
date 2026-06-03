#include "domain/Vehicle.h"

#include "exceptions/DomainException.h"

namespace domain {

Vehicle::Vehicle(int id, std::string inventoryNo, std::string name,
                 util::Money initialCost, util::Date acquiredOn,
                 int usefulLifeMonths, int departmentId,
                 std::string licensePlate, int mileage, int expectedTotalMileage)
    : FixedAsset(id, std::move(inventoryNo), std::move(name), initialCost, acquiredOn,
                 usefulLifeMonths, departmentId),
      licensePlate_(std::move(licensePlate)),
      mileage_(mileage),
      expectedTotalMileage_(expectedTotalMileage) {
    if (mileage_ < 0) {
        throw ValidationException("Пробег не может быть отрицательным");
    }
    if (expectedTotalMileage_ <= 0) {
        throw ValidationException("Ресурс пробега должен быть положительным");
    }
}

// Амортизация по пробегу: износ = initialCost * (mileage / expectedTotalMileage),
// с потолком в initialCost (clamp внутри depreciationFromRatio).
util::Money Vehicle::calculateDepreciation(util::Date /*onDate*/) const {
    return depreciationFromRatio(static_cast<double>(mileage_) /
                                 static_cast<double>(expectedTotalMileage_));
}

nlohmann::json Vehicle::toJson() const {
    nlohmann::json j = FixedAsset::toJson();
    j["licensePlate"]         = licensePlate_;
    j["mileage"]              = mileage_;
    j["expectedTotalMileage"] = expectedTotalMileage_;
    return j;
}

void Vehicle::addMileage(int km) {
    if (km < 0) {
        throw ValidationException("Прирост пробега не может быть отрицательным");
    }
    mileage_ += km;
}

}  // namespace domain
