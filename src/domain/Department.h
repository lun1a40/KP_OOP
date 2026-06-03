#pragma once

#include <string>
#include <stdexcept>

#include "nlohmann/json.hpp"

namespace domain {

// Типы цехов/подразделений игрушечной фабрики. Production оставлен
// как общий тип для обратной совместимости со старыми данными.
enum class DepartmentType {
    Production,       // производство (общий)
    Molding,          // литейный цех (ТПА)
    SoftToy,          // цех мягкой игрушки
    Painting,         // покрасочный цех
    Assembly,         // сборочный цех
    Packaging,        // упаковочный цех
    QualityControl,   // отдел технического контроля (ОТК)
    Warehouse,        // склад
    Admin             // администрация
};

inline std::string toString(DepartmentType t) {
    switch (t) {
        case DepartmentType::Production:     return "Production";
        case DepartmentType::Molding:        return "Molding";
        case DepartmentType::SoftToy:        return "SoftToy";
        case DepartmentType::Painting:       return "Painting";
        case DepartmentType::Assembly:       return "Assembly";
        case DepartmentType::Packaging:      return "Packaging";
        case DepartmentType::QualityControl: return "QualityControl";
        case DepartmentType::Warehouse:      return "Warehouse";
        case DepartmentType::Admin:          return "Admin";
    }
    return "Production";
}

inline DepartmentType departmentTypeFromString(const std::string& s) {
    if (s == "Production")     return DepartmentType::Production;
    if (s == "Molding")        return DepartmentType::Molding;
    if (s == "SoftToy")        return DepartmentType::SoftToy;
    if (s == "Painting")       return DepartmentType::Painting;
    if (s == "Assembly")       return DepartmentType::Assembly;
    if (s == "Packaging")      return DepartmentType::Packaging;
    if (s == "QualityControl") return DepartmentType::QualityControl;
    if (s == "Warehouse")      return DepartmentType::Warehouse;
    if (s == "Admin")          return DepartmentType::Admin;
    throw std::invalid_argument("Неизвестный DepartmentType: " + s);
}

// Подразделение предприятия. Ассоциировано с фондами через departmentId.
class Department {
public:
    Department(int id, std::string name, std::string head, DepartmentType type);

    int                id()   const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& head() const { return head_; }
    DepartmentType     type() const { return type_; }

    void setId(int id) { id_ = id; }

    nlohmann::json toJson() const;

private:
    int            id_;
    std::string    name_;
    std::string    head_;
    DepartmentType type_;
};

}  // namespace domain
