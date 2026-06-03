#pragma once

#include <string>
#include <vector>

#include "repository/IMovementRepository.h"
#include "domain/AssetMovement.h"
#include "util/Date.h"

namespace service {

// Ведёт журнал движений фондов. Записи создаются при учётных операциях
// (поступление, перемещение, ремонт, списание) и читаются в отчётах.
class MovementService {
public:
    explicit MovementService(repository::IMovementRepository& repo);

    void recordAcquisition(int assetId, int departmentId,
                           const std::string& performedBy = "admin");
    void recordTransfer(int assetId, int fromDepartmentId, int toDepartmentId,
                        const std::string& note, const std::string& performedBy = "admin");
    void recordRepair(int assetId, domain::MovementType type,
                      const std::string& note, const std::string& performedBy = "admin");
    void recordWriteOff(int assetId, const std::string& reason,
                        const std::string& performedBy = "admin");

    std::vector<const domain::AssetMovement*> history(int assetId) const;
    std::vector<const domain::AssetMovement*> inPeriod(util::Date from, util::Date to) const;
    std::vector<const domain::AssetMovement*> all() const;

private:
    repository::IMovementRepository& repo_;
};

}  // namespace service
