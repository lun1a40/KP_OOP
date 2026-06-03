#pragma once

#include <string>
#include <vector>

#include "repository/IAssetRepository.h"
#include "repository/IDepartmentRepository.h"
#include "util/Date.h"
#include "util/Money.h"

namespace service {

// Аналитика поверх фондов. Работает полиморфно через базовый FixedAsset:
// каждый тип считает амортизацию по-своему, отчёт об этом не знает.
class ReportService {
public:
    ReportService(repository::IAssetRepository& assets,
                  repository::IDepartmentRepository& departments);

    struct DepreciationRow {
        int         assetId;
        std::string inventoryNo;
        std::string name;
        std::string category;
        util::Money initialCost;
        util::Money accumulated;   // накопленная амортизация
        util::Money residual;      // остаточная стоимость
    };

    struct DepreciationReport {
        util::Date  onDate;
        std::vector<DepreciationRow> rows;
        util::Money totalInitial;
        util::Money totalAccumulated;
        util::Money totalResidual;
    };

    DepreciationReport depreciationReport(util::Date onDate);

    struct DeptSummary {
        int         departmentId;
        std::string departmentName;
        int         assetCount;
        util::Money totalResidual;
    };

    std::vector<DeptSummary> byDepartment(util::Date onDate);

    // Производственные мощности по видам игрушек: свёртка производственного
    // оборудования (ProductionEquipment) по выпускаемому виду игрушки.
    struct ToyCapacityRow {
        std::string toyCategory;            // код вида игрушки ("plastic"/...)
        int         lineCount;              // число линий
        long long   totalCapacityPerHour;   // суммарная производительность, игрушек/час
    };

    std::vector<ToyCapacityRow> capacityByToyCategory();

private:
    repository::IAssetRepository&      assets_;
    repository::IDepartmentRepository& departments_;
};

}  // namespace service
