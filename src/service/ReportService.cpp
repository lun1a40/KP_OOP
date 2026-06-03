#include "service/ReportService.h"

#include <map>

#include "domain/ProductionEquipment.h"

namespace service {

using domain::FixedAsset;
using domain::ProductionEquipment;

ReportService::ReportService(repository::IAssetRepository& assets,
                             repository::IDepartmentRepository& departments)
    : assets_(assets), departments_(departments) {}

ReportService::DepreciationReport ReportService::depreciationReport(util::Date onDate) {
    DepreciationReport report{onDate};   // остальные поля — value-init (vector пуст, Money = 0)

    for (const auto* a : assets_.all()) {
        const util::Money accumulated = a->calculateDepreciation(onDate);  // полиморфизм
        const util::Money residual    = a->residualValue(onDate);

        report.rows.push_back(DepreciationRow{
            a->id(), a->inventoryNo(), a->name(), a->category(),
            a->initialCost(), accumulated, residual});

        report.totalInitial     += a->initialCost();
        report.totalAccumulated += accumulated;
        report.totalResidual    += residual;
    }
    return report;
}

std::vector<ReportService::DeptSummary> ReportService::byDepartment(util::Date onDate) {
    // Свёртка по departmentId с сохранением порядка появления подразделений.
    std::map<int, DeptSummary> acc;

    for (const auto* a : assets_.all()) {
        const int deptId = a->departmentId();
        auto it = acc.find(deptId);
        if (it == acc.end()) {
            const auto* dept = departments_.find(deptId);
            DeptSummary s;
            s.departmentId   = deptId;
            s.departmentName = dept ? dept->name() : ("(подразделение #" + std::to_string(deptId) + ")");
            s.assetCount     = 0;
            it = acc.emplace(deptId, s).first;
        }
        it->second.assetCount += 1;
        it->second.totalResidual += a->residualValue(onDate);
    }

    std::vector<DeptSummary> result;
    result.reserve(acc.size());
    for (auto& [id, summary] : acc) {
        result.push_back(summary);
    }
    return result;
}

std::vector<ReportService::ToyCapacityRow> ReportService::capacityByToyCategory() {
    std::map<std::string, ToyCapacityRow> acc;

    for (auto* a : assets_.all()) {
        // Учитываем только производственное оборудование — у него есть
        // вид игрушки и производительность (полиморфное приведение типа).
        const auto* pe = dynamic_cast<const ProductionEquipment*>(a);
        if (!pe) continue;

        const std::string cat = domain::toString(pe->toyCategory());
        auto it = acc.find(cat);
        if (it == acc.end()) {
            it = acc.emplace(cat, ToyCapacityRow{cat, 0, 0}).first;
        }
        it->second.lineCount += 1;
        it->second.totalCapacityPerHour += pe->capacityPerHour();
    }

    std::vector<ToyCapacityRow> result;
    result.reserve(acc.size());
    for (auto& [cat, row] : acc) {
        result.push_back(row);
    }
    return result;
}

}  // namespace service
