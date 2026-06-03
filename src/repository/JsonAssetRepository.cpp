#include "repository/JsonAssetRepository.h"

#include "repository/JsonFile.h"
#include "repository/JsonMapper.h"
#include "exceptions/DomainException.h"

namespace repository {

JsonAssetRepository::JsonAssetRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    load();
}

void JsonAssetRepository::load() {
    assets_.clear();
    const nlohmann::json arr = readArray(filePath_);
    for (const auto& item : arr) {
        auto asset = JsonMapper::assetFromJson(item);
        const int id = asset->id();
        assets_[id] = std::move(asset);
    }
}

domain::FixedAsset* JsonAssetRepository::find(int id) {
    auto it = assets_.find(id);
    return it == assets_.end() ? nullptr : it->second.get();
}

const domain::FixedAsset* JsonAssetRepository::find(int id) const {
    auto it = assets_.find(id);
    return it == assets_.end() ? nullptr : it->second.get();
}

std::vector<domain::FixedAsset*> JsonAssetRepository::all() {
    std::vector<domain::FixedAsset*> result;
    result.reserve(assets_.size());
    for (auto& [id, ptr] : assets_) {
        result.push_back(ptr.get());
    }
    return result;
}

int JsonAssetRepository::add(std::unique_ptr<domain::FixedAsset> asset) {
    int id = asset->id();
    if (id == 0) {
        id = nextId();
        asset->setId(id);
    }
    if (assets_.count(id)) {
        throw domain::ValidationException("Фонд с id=" + std::to_string(id) + " уже существует");
    }
    assets_[id] = std::move(asset);
    persist();
    return id;
}

void JsonAssetRepository::remove(int id) {
    if (assets_.erase(id) == 0) {
        throw domain::NotFoundException("Asset", id);
    }
    persist();
}

void JsonAssetRepository::persist() {
    std::vector<domain::FixedAsset*> ptrs = all();
    writeJson(filePath_, JsonMapper::toJsonArray(ptrs));
}

int JsonAssetRepository::nextId() const {
    return assets_.empty() ? 1 : assets_.rbegin()->first + 1;
}

}  // namespace repository
