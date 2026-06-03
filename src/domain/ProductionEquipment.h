#pragma once

#include "domain/FixedAsset.h"
#include "domain/ToyCategory.h"

namespace domain {

// Производственное оборудование: термопластавтоматы, швейные линии мягкой
// игрушки, конвейеры покраски/упаковки, станки ЧПУ. Привязано к виду
// выпускаемой игрушки и её возрастной маркировке (ТР ТС 008/2011).
// Амортизация — по фактической наработке часов.
class ProductionEquipment : public FixedAsset {
public:
    ProductionEquipment(int id, std::string inventoryNo, std::string name,
                        util::Money initialCost, util::Date acquiredOn,
                        int usefulLifeMonths, int departmentId,
                        std::string model, int capacityPerHour, int runHours,
                        ToyCategory toyCategory, AgeGroup ageGroup);

    util::Money    calculateDepreciation(util::Date onDate) const override;
    std::string    category() const override { return "production"; }
    nlohmann::json toJson() const override;

    const std::string& model()          const { return model_; }
    int                capacityPerHour() const { return capacityPerHour_; }   // игрушек/час
    int                runHours()        const { return runHours_; }
    ToyCategory        toyCategory()     const { return toyCategory_; }
    AgeGroup           ageGroup()        const { return ageGroup_; }
    void incrementRunHours(int hours);

private:
    std::string model_;
    int         capacityPerHour_;   // игрушек/час
    int         runHours_;          // фактическая наработка
    ToyCategory toyCategory_;       // вид выпускаемой игрушки
    AgeGroup    ageGroup_;          // возрастная маркировка
};

}  // namespace domain
