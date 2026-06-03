#include "service/MovementService.h"

#include <optional>

namespace service {

using domain::AssetMovement;
using domain::MovementType;

MovementService::MovementService(repository::IMovementRepository& repo) : repo_(repo) {}

void MovementService::recordAcquisition(int assetId, int departmentId,
                                        const std::string& performedBy) {
    repo_.add(AssetMovement(0, assetId, MovementType::Acquired,
                            std::nullopt, departmentId,
                            util::Date::today(), "Поступление на учёт", performedBy));
}

void MovementService::recordTransfer(int assetId, int fromDepartmentId, int toDepartmentId,
                                     const std::string& note, const std::string& performedBy) {
    repo_.add(AssetMovement(0, assetId, MovementType::Transferred,
                            fromDepartmentId, toDepartmentId,
                            util::Date::today(), note, performedBy));
}

void MovementService::recordRepair(int assetId, MovementType type,
                                   const std::string& note, const std::string& performedBy) {
    repo_.add(AssetMovement(0, assetId, type,
                            std::nullopt, std::nullopt,
                            util::Date::today(), note, performedBy));
}

void MovementService::recordWriteOff(int assetId, const std::string& reason,
                                     const std::string& performedBy) {
    repo_.add(AssetMovement(0, assetId, MovementType::WrittenOff,
                            std::nullopt, std::nullopt,
                            util::Date::today(), reason, performedBy));
}

std::vector<const AssetMovement*> MovementService::history(int assetId) const {
    std::vector<const AssetMovement*> result;
    for (const auto* m : repo_.all()) {
        if (m->assetId() == assetId) {
            result.push_back(m);
        }
    }
    return result;
}

std::vector<const AssetMovement*> MovementService::inPeriod(util::Date from, util::Date to) const {
    std::vector<const AssetMovement*> result;
    for (const auto* m : repo_.all()) {
        if (m->date() >= from && m->date() <= to) {
            result.push_back(m);
        }
    }
    return result;
}

std::vector<const AssetMovement*> MovementService::all() const {
    return repo_.all();
}

}  // namespace service
