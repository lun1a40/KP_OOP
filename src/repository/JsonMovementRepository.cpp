#include "repository/JsonMovementRepository.h"

#include "repository/JsonFile.h"
#include "repository/JsonMapper.h"

namespace repository {

JsonMovementRepository::JsonMovementRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    load();
}

void JsonMovementRepository::load() {
    movements_.clear();
    maxId_ = 0;
    const nlohmann::json arr = readArray(filePath_);
    for (const auto& item : arr) {
        domain::AssetMovement m = JsonMapper::movementFromJson(item);
        if (m.id() > maxId_) {
            maxId_ = m.id();
        }
        movements_.push_back(std::move(m));
    }
}

std::vector<const domain::AssetMovement*> JsonMovementRepository::all() const {
    std::vector<const domain::AssetMovement*> result;
    result.reserve(movements_.size());
    for (const auto& m : movements_) {
        result.push_back(&m);
    }
    return result;
}

int JsonMovementRepository::add(domain::AssetMovement movement) {
    int id = movement.id();
    if (id == 0) {
        id = nextId();
        movement.setId(id);
    }
    if (id > maxId_) {
        maxId_ = id;
    }
    movements_.push_back(std::move(movement));
    persist();
    return id;
}

void JsonMovementRepository::persist() {
    writeJson(filePath_, JsonMapper::toJsonArray(movements_));
}

int JsonMovementRepository::nextId() const {
    return maxId_ + 1;
}

}  // namespace repository
