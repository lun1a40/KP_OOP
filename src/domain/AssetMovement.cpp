#include "domain/AssetMovement.h"

namespace domain {

AssetMovement::AssetMovement(int id, int assetId, MovementType type,
                             std::optional<int> fromDepartmentId,
                             std::optional<int> toDepartmentId,
                             util::Date date, std::string note, std::string performedBy)
    : id_(id),
      assetId_(assetId),
      type_(type),
      fromDepartmentId_(fromDepartmentId),
      toDepartmentId_(toDepartmentId),
      date_(date),
      note_(std::move(note)),
      performedBy_(std::move(performedBy)) {}

nlohmann::json AssetMovement::toJson() const {
    nlohmann::json j = {
        {"id",          id_},
        {"assetId",     assetId_},
        {"type",        toString(type_)},
        {"date",        date_.toIso()},
        {"note",        note_},
        {"performedBy", performedBy_},
    };
    j["fromDepartmentId"] = fromDepartmentId_ ? nlohmann::json(*fromDepartmentId_)
                                              : nlohmann::json(nullptr);
    j["toDepartmentId"]   = toDepartmentId_   ? nlohmann::json(*toDepartmentId_)
                                              : nlohmann::json(nullptr);
    return j;
}

}  // namespace domain
