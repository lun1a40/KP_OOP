#pragma once

#include <optional>
#include <string>

#include "domain/AssetStatus.h"

namespace service {

// Критерии выборки фондов для AssetService::list. Любое поле пустое — не фильтруем.
struct AssetFilter {
    std::optional<std::string>        category;      // "production"/"building"/...
    std::optional<int>                departmentId;
    std::optional<domain::AssetStatus> status;
    std::optional<std::string>        query;         // подстрока в name / inventoryNo
};

}  // namespace service
