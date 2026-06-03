#include "domain/OfficeEquipment.h"

namespace domain {

OfficeEquipment::OfficeEquipment(int id, std::string inventoryNo, std::string name,
                                 util::Money initialCost, util::Date acquiredOn,
                                 int usefulLifeMonths, int departmentId,
                                 std::string serialNumber)
    : FixedAsset(id, std::move(inventoryNo), std::move(name), initialCost, acquiredOn,
                 usefulLifeMonths, departmentId),
      serialNumber_(std::move(serialNumber)) {}

// Метод суммы чисел лет: за год i (1..N) списывается доля (N - i + 1) / SumYears,
// где SumYears = N*(N+1)/2. Незавершённый год учитывается пропорционально месяцам.
util::Money OfficeEquipment::calculateDepreciation(util::Date onDate) const {
    int monthsElapsed = acquiredOn_.monthsBetween(onDate);
    if (monthsElapsed <= 0) {
        return util::Money(0);
    }
    // По истечении срока службы — полный износ (страховка от ошибок округления).
    if (monthsElapsed >= usefulLifeMonths_) {
        return initialCost_;
    }

    const int N = usefulLifeMonths_ / 12 > 0 ? usefulLifeMonths_ / 12 : 1;
    const double sumYears = static_cast<double>(N) * (N + 1) / 2.0;

    const int fullYears       = monthsElapsed / 12;
    const int remainderMonths = monthsElapsed % 12;

    double coefficient = 0.0;
    for (int i = 1; i <= fullYears && i <= N; ++i) {
        coefficient += static_cast<double>(N - i + 1) / sumYears;
    }
    // Доля текущего (незавершённого) года.
    const int currentYear = fullYears + 1;
    if (currentYear <= N && remainderMonths > 0) {
        const double yearShare = static_cast<double>(N - currentYear + 1) / sumYears;
        coefficient += yearShare * (static_cast<double>(remainderMonths) / 12.0);
    }

    return depreciationFromRatio(coefficient);
}

nlohmann::json OfficeEquipment::toJson() const {
    nlohmann::json j = FixedAsset::toJson();
    j["serialNumber"] = serialNumber_;
    return j;
}

}  // namespace domain
