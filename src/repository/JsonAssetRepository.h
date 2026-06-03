#pragma once

#include <map>
#include <memory>
#include <string>

#include "repository/IAssetRepository.h"

namespace repository {

// Реализация IAssetRepository поверх JSON-файла.
// Фонды держатся в памяти как unique_ptr<FixedAsset>: полиморфное
// владение без копирования. std::map даёт детерминированный порядок по id
// при сериализации.
class JsonAssetRepository : public IAssetRepository {
public:
    explicit JsonAssetRepository(std::string filePath);

    domain::FixedAsset*       find(int id) override;
    const domain::FixedAsset* find(int id) const override;
    std::vector<domain::FixedAsset*> all() override;
    int  add(std::unique_ptr<domain::FixedAsset> asset) override;
    void remove(int id) override;
    void persist() override;
    int  nextId() const override;

private:
    void load();

    std::string filePath_;
    std::map<int, std::unique_ptr<domain::FixedAsset>> assets_;
};

}  // namespace repository
