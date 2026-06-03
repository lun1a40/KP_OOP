#pragma once

#include "domain/FixedAsset.h"

namespace domain {

// Здания: производственные цеха, склады, административные корпуса.
// Амортизация — линейная (равномерная по сроку службы).
class Building : public FixedAsset {
public:
    Building(int id, std::string inventoryNo, std::string name,
             util::Money initialCost, util::Date acquiredOn,
             int usefulLifeMonths, int departmentId,
             double area, std::string address);

    util::Money    calculateDepreciation(util::Date onDate) const override;
    std::string    category() const override { return "building"; }
    nlohmann::json toJson() const override;

    double             area()    const { return area_; }
    const std::string& address() const { return address_; }

private:
    double      area_;       // кв.м
    std::string address_;
};

}  // namespace domain
