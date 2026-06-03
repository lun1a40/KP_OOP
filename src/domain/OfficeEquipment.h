#pragma once

#include "domain/FixedAsset.h"

namespace domain {

// Офисная техника: компьютеры бухгалтерии, принтеры, копиры.
// Амортизация — ускоренная, методом суммы чисел лет (sum-of-years-digits).
class OfficeEquipment : public FixedAsset {
public:
    OfficeEquipment(int id, std::string inventoryNo, std::string name,
                    util::Money initialCost, util::Date acquiredOn,
                    int usefulLifeMonths, int departmentId,
                    std::string serialNumber);

    util::Money    calculateDepreciation(util::Date onDate) const override;
    std::string    category() const override { return "office"; }
    nlohmann::json toJson() const override;

    const std::string& serialNumber() const { return serialNumber_; }

private:
    std::string serialNumber_;
};

}  // namespace domain
