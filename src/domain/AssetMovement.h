#pragma once

#include <string>
#include <optional>

#include "util/Date.h"
#include "domain/MovementType.h"
#include "nlohmann/json.hpp"

namespace domain {

// Запись журнала движений фонда. Неизменяема после создания (immutable):
// поля только читаются. Журнал — агрегация по отношению к FixedAsset.
class AssetMovement {
public:
    AssetMovement(int id, int assetId, MovementType type,
                  std::optional<int> fromDepartmentId,
                  std::optional<int> toDepartmentId,
                  util::Date date, std::string note, std::string performedBy);

    int                       id()               const { return id_; }
    int                       assetId()          const { return assetId_; }
    MovementType              type()             const { return type_; }
    std::optional<int>        fromDepartmentId() const { return fromDepartmentId_; }
    std::optional<int>        toDepartmentId()   const { return toDepartmentId_; }
    const util::Date&         date()             const { return date_; }
    const std::string&        note()             const { return note_; }
    const std::string&        performedBy()      const { return performedBy_; }

    void setId(int id) { id_ = id; }

    nlohmann::json toJson() const;

private:
    int                id_;
    int                assetId_;
    MovementType       type_;
    std::optional<int> fromDepartmentId_;
    std::optional<int> toDepartmentId_;
    util::Date         date_;
    std::string        note_;
    std::string        performedBy_;
};

}  // namespace domain
