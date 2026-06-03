#pragma once

#include "domain/FixedAsset.h"

namespace domain {

// Транспорт: электропогрузчики, фургоны доставки.
// Амортизация — по пробегу.
class Vehicle : public FixedAsset {
public:
    Vehicle(int id, std::string inventoryNo, std::string name,
            util::Money initialCost, util::Date acquiredOn,
            int usefulLifeMonths, int departmentId,
            std::string licensePlate, int mileage, int expectedTotalMileage);

    util::Money    calculateDepreciation(util::Date onDate) const override;
    std::string    category() const override { return "vehicle"; }
    nlohmann::json toJson() const override;

    const std::string& licensePlate()         const { return licensePlate_; }
    int                mileage()              const { return mileage_; }
    int                expectedTotalMileage() const { return expectedTotalMileage_; }
    void addMileage(int km);

private:
    std::string licensePlate_;
    int mileage_;                // км
    int expectedTotalMileage_;   // ресурс пробега
};

}  // namespace domain
