#pragma once

#include <memory>
#include <string>
#include <vector>

#include "repository/IAssetRepository.h"
#include "repository/IDepartmentRepository.h"
#include "service/MovementService.h"
#include "service/AssetFilter.h"
#include "domain/FixedAsset.h"
#include "util/Money.h"

namespace service {

// Учётные операции над фондами: регистрация, изменение, перемещение,
// ремонт, списание. Валидирует бизнес-правила и ведёт журнал через
// MovementService. Контроллеры обращаются только сюда, не к репозиториям.
class AssetService {
public:
    AssetService(repository::IAssetRepository& assets,
                 repository::IDepartmentRepository& departments,
                 MovementService& movements);

    int  add(std::unique_ptr<domain::FixedAsset> asset);          // → id
    void update(int id, std::unique_ptr<domain::FixedAsset> newData);
    void remove(int id);                                          // только если списан

    void transfer(int id, int newDepartmentId, const std::string& note);
    void sendToRepair(int id, const std::string& note);
    void backFromRepair(int id, util::Money repairCost, const std::string& description);
    void writeOff(int id, const std::string& reason);

    const domain::FixedAsset& get(int id) const;
    std::vector<domain::FixedAsset*> list(const AssetFilter& filter);

private:
    domain::FixedAsset&       require(int id);
    const domain::FixedAsset& require(int id) const;
    void ensureDepartmentExists(int departmentId) const;
    void ensureUniqueInventoryNo(const std::string& inventoryNo, int excludeId) const;

    repository::IAssetRepository&      assets_;
    repository::IDepartmentRepository& departments_;
    MovementService&                   movements_;
};

}  // namespace service
