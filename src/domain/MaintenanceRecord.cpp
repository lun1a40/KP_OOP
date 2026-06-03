#include "domain/MaintenanceRecord.h"

namespace domain {

MaintenanceRecord::MaintenanceRecord(int id, util::Date date, MaintenanceType type,
                                     util::Money cost, std::string description,
                                     std::string performedBy)
    : id_(id),
      date_(date),
      type_(type),
      cost_(cost),
      description_(std::move(description)),
      performedBy_(std::move(performedBy)) {}

nlohmann::json MaintenanceRecord::toJson() const {
    return {
        {"id",          id_},
        {"date",        date_.toIso()},
        {"type",        toString(type_)},
        {"cost",        cost_.kopecks()},
        {"description", description_},
        {"performedBy", performedBy_},
    };
}

}  // namespace domain
