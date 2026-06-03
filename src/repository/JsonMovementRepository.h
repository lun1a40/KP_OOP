#pragma once

#include <string>
#include <vector>

#include "repository/IMovementRepository.h"

namespace repository {

// Реализация IMovementRepository поверх JSON-файла. Журнал append-only.
class JsonMovementRepository : public IMovementRepository {
public:
    explicit JsonMovementRepository(std::string filePath);

    std::vector<const domain::AssetMovement*> all() const override;
    int  add(domain::AssetMovement movement) override;
    void persist() override;
    int  nextId() const override;

private:
    void load();

    std::string filePath_;
    std::vector<domain::AssetMovement> movements_;
    int maxId_ = 0;
};

}  // namespace repository
