#pragma once

#include <string>
#include <stdexcept>

namespace domain {

// Состояние основного фонда.
enum class AssetStatus { InUse, InRepair, WrittenOff };

inline std::string toString(AssetStatus s) {
    switch (s) {
        case AssetStatus::InUse:      return "InUse";
        case AssetStatus::InRepair:   return "InRepair";
        case AssetStatus::WrittenOff: return "WrittenOff";
    }
    return "InUse";
}

inline AssetStatus assetStatusFromString(const std::string& s) {
    if (s == "InUse")      return AssetStatus::InUse;
    if (s == "InRepair")   return AssetStatus::InRepair;
    if (s == "WrittenOff") return AssetStatus::WrittenOff;
    throw std::invalid_argument("Неизвестный AssetStatus: " + s);
}

}  // namespace domain
