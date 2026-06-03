#pragma once

#include <string>
#include <stdexcept>

namespace domain {

// Вид выпускаемой игрушки — определяет технологию производства.
enum class ToyCategory { Plastic, Plush, Wooden, BoardGame, Electronic };

inline std::string toString(ToyCategory c) {
    switch (c) {
        case ToyCategory::Plastic:    return "plastic";
        case ToyCategory::Plush:      return "plush";
        case ToyCategory::Wooden:     return "wooden";
        case ToyCategory::BoardGame:  return "board";
        case ToyCategory::Electronic: return "electronic";
    }
    return "plastic";
}

inline ToyCategory toyCategoryFromString(const std::string& s) {
    if (s == "plastic")    return ToyCategory::Plastic;
    if (s == "plush")      return ToyCategory::Plush;
    if (s == "wooden")     return ToyCategory::Wooden;
    if (s == "board")      return ToyCategory::BoardGame;
    if (s == "electronic") return ToyCategory::Electronic;
    throw std::invalid_argument("Неизвестный ToyCategory: " + s);
}

// Возрастная маркировка детской игрушки (ТР ТС 008/2011).
enum class AgeGroup { From0, From3, From6, From14 };

inline std::string toString(AgeGroup a) {
    switch (a) {
        case AgeGroup::From0:  return "0+";
        case AgeGroup::From3:  return "3+";
        case AgeGroup::From6:  return "6+";
        case AgeGroup::From14: return "14+";
    }
    return "0+";
}

inline AgeGroup ageGroupFromString(const std::string& s) {
    if (s == "0+")  return AgeGroup::From0;
    if (s == "3+")  return AgeGroup::From3;
    if (s == "6+")  return AgeGroup::From6;
    if (s == "14+") return AgeGroup::From14;
    throw std::invalid_argument("Неизвестная возрастная группа: " + s);
}

}  // namespace domain
