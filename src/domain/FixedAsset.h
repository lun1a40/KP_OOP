#pragma once

#include <string>
#include <vector>
#include <ostream>

#include "util/Date.h"
#include "util/Money.h"
#include "domain/AssetStatus.h"
#include "domain/MaintenanceRecord.h"
#include "nlohmann/json.hpp"

namespace domain {

// Абстрактный базовый класс основного фонда.
// Определяет полиморфный API (calculateDepreciation/category/toJson),
// который переопределяют 4 наследника.
class FixedAsset {
public:
    FixedAsset(int id, std::string inventoryNo, std::string name,
               util::Money initialCost, util::Date acquiredOn,
               int usefulLifeMonths, int departmentId);

    virtual ~FixedAsset() = default;

    // --- Полиморфный API ---
    // Амортизация на дату. Алгоритм зависит от типа фонда.
    virtual util::Money calculateDepreciation(util::Date onDate) const = 0;
    // Машинный код категории ("production"/"building"/"vehicle"/"office").
    virtual std::string category() const = 0;
    // Базовая реализация пишет общие поля; наследники вызывают её и дополняют.
    virtual nlohmann::json toJson() const;

    // --- Геттеры ---
    int                 id()               const { return id_; }
    const std::string&  inventoryNo()      const { return inventoryNo_; }
    const std::string&  name()             const { return name_; }
    util::Money         initialCost()      const { return initialCost_; }
    util::Date          acquiredOn()       const { return acquiredOn_; }
    int                 usefulLifeMonths() const { return usefulLifeMonths_; }
    AssetStatus         status()           const { return status_; }
    int                 departmentId()     const { return departmentId_; }
    const std::vector<MaintenanceRecord>& maintenance() const { return maintenance_; }

    // --- Изменение состояния (с валидацией) ---
    void setId(int id) { id_ = id; }
    void setStatus(AssetStatus s);
    void setDepartment(int newDepartmentId);
    void addMaintenance(MaintenanceRecord record);

    // Остаточная стоимость = initialCost − амортизация (не ниже нуля).
    util::Money residualValue(util::Date onDate) const;

    // --- Операторы ---
    bool operator==(const FixedAsset& o) const;   // по inventoryNo
    bool operator<(const FixedAsset& o)  const;    // по id
    friend std::ostream& operator<<(std::ostream& os, const FixedAsset& a);

protected:
    // Утилита для наследников: initialCost * clamp(ratio, 0..1).
    util::Money depreciationFromRatio(double ratio) const;

    int                            id_;
    std::string                    inventoryNo_;
    std::string                    name_;
    util::Money                    initialCost_;
    util::Date                     acquiredOn_;
    int                            usefulLifeMonths_;
    AssetStatus                    status_;
    int                            departmentId_;
    std::vector<MaintenanceRecord> maintenance_;
};

}  // namespace domain
