#pragma once

#include <string>
#include <stdexcept>

#include "util/Date.h"
#include "util/Money.h"
#include "nlohmann/json.hpp"

namespace domain {

enum class MaintenanceType { Planned, Emergency };

inline std::string toString(MaintenanceType t) {
    switch (t) {
        case MaintenanceType::Planned:   return "Planned";
        case MaintenanceType::Emergency: return "Emergency";
    }
    return "Planned";
}

inline MaintenanceType maintenanceTypeFromString(const std::string& s) {
    if (s == "Planned")   return MaintenanceType::Planned;
    if (s == "Emergency") return MaintenanceType::Emergency;
    throw std::invalid_argument("Неизвестный MaintenanceType: " + s);
}

// Запись журнала ТО/ремонта. Входит в композицию FixedAsset
// (vector<MaintenanceRecord> — время жизни записей управляется фондом).
class MaintenanceRecord {
public:
    MaintenanceRecord(int id, util::Date date, MaintenanceType type,
                      util::Money cost, std::string description, std::string performedBy);

    int                  id()          const { return id_; }
    const util::Date&    date()        const { return date_; }
    MaintenanceType      type()        const { return type_; }
    util::Money          cost()        const { return cost_; }
    const std::string&   description() const { return description_; }
    const std::string&   performedBy() const { return performedBy_; }

    nlohmann::json toJson() const;

private:
    int             id_;
    util::Date      date_;
    MaintenanceType type_;
    util::Money     cost_;
    std::string     description_;
    std::string     performedBy_;
};

}  // namespace domain
