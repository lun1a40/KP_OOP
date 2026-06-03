#pragma once

#include <string>
#include <stdexcept>

namespace domain {

// Тип записи в журнале движений фонда.
enum class MovementType {
    Acquired,        // поступление
    Transferred,     // перемещение между подразделениями
    SentToRepair,    // отправлен на ремонт
    BackFromRepair,  // возврат из ремонта
    WrittenOff       // списание
};

inline std::string toString(MovementType t) {
    switch (t) {
        case MovementType::Acquired:       return "Acquired";
        case MovementType::Transferred:    return "Transferred";
        case MovementType::SentToRepair:   return "SentToRepair";
        case MovementType::BackFromRepair: return "BackFromRepair";
        case MovementType::WrittenOff:     return "WrittenOff";
    }
    return "Acquired";
}

inline MovementType movementTypeFromString(const std::string& s) {
    if (s == "Acquired")       return MovementType::Acquired;
    if (s == "Transferred")    return MovementType::Transferred;
    if (s == "SentToRepair")   return MovementType::SentToRepair;
    if (s == "BackFromRepair") return MovementType::BackFromRepair;
    if (s == "WrittenOff")     return MovementType::WrittenOff;
    throw std::invalid_argument("Неизвестный MovementType: " + s);
}

}  // namespace domain
