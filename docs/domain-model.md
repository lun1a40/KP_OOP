# Доменная модель

## Утилитарные типы

### `util::Date`
Собственный класс даты, чтобы не тащить `<chrono>`-нагромождения и иметь предсказуемую сериализацию.

```cpp
class Date {
public:
    Date(int year, int month, int day);
    static Date today();
    static Date fromIso(const std::string&);    // "2026-06-03"
    std::string toIso() const;

    int year() const;  int month() const;  int day() const;
    int monthsBetween(const Date& other) const;
    int daysBetween(const Date& other) const;

    bool operator==(const Date&) const;
    bool operator<(const Date&) const;
    friend std::ostream& operator<<(std::ostream&, const Date&);
};
```

### `util::Money`
Тонкая обёртка над `int64_t` (копейки). Не наследник `FixedAsset`, просто значение.

```cpp
class Money {
    int64_t kopecks_;
public:
    explicit Money(int64_t kopecks);
    static Money fromRubles(double r);     // конвертация на границе
    int64_t kopecks() const;
    double  rubles()  const;
    std::string format() const;            // "12 500,00 ₽"

    Money operator+(Money) const;
    Money operator-(Money) const;
    Money operator*(double) const;
    bool  operator<(Money) const;
};
```

### `AssetStatus` (enum class)
```cpp
enum class AssetStatus { InUse, InRepair, WrittenOff };
```

### `MovementType` (enum class)
```cpp
enum class MovementType {
    Acquired,        // поступление
    Transferred,     // перемещение между подразделениями
    SentToRepair,    // отправлен на ремонт
    BackFromRepair,  // возврат из ремонта
    WrittenOff       // списание
};
```

---

## Основные классы

### `FixedAsset` (абстрактный)

Основной базовый класс. Содержит общие для всех типов фондов поля и определяет полиморфный API.

```cpp
class FixedAsset {
protected:
    int          id_;
    std::string  inventoryNo_;
    std::string  name_;
    Money        initialCost_;
    Date         acquiredOn_;
    int          usefulLifeMonths_;
    AssetStatus  status_;
    int          departmentId_;
    std::vector<MaintenanceRecord> maintenance_;

public:
    FixedAsset(int id, std::string inventoryNo, std::string name,
               Money initialCost, Date acquiredOn,
               int usefulLifeMonths, int departmentId);
    virtual ~FixedAsset() = default;

    // Полиморфный API
    virtual Money       calculateDepreciation(Date onDate) const = 0;
    virtual std::string category() const = 0;
    virtual nlohmann::json toJson() const;     // базовый — общие поля

    // Геттеры
    int           id()             const { return id_; }
    const auto&   inventoryNo()    const { return inventoryNo_; }
    const auto&   name()           const { return name_; }
    Money         initialCost()    const { return initialCost_; }
    Date          acquiredOn()     const { return acquiredOn_; }
    int           usefulLifeMonths() const { return usefulLifeMonths_; }
    AssetStatus   status()         const { return status_; }
    int           departmentId()   const { return departmentId_; }
    const auto&   maintenance()    const { return maintenance_; }

    // Изменение состояния (с валидацией)
    void setStatus(AssetStatus s);
    void setDepartment(int newDeptId);   // используется при перемещении
    void addMaintenance(MaintenanceRecord r);

    // Удобная производная величина
    Money residualValue(Date onDate) const;  // = initialCost - depreciation

    // Операторы
    bool operator==(const FixedAsset& other) const;   // по inventoryNo
    bool operator< (const FixedAsset& other) const;   // по id
    friend std::ostream& operator<<(std::ostream&, const FixedAsset&);
};
```

### `ProductionEquipment : FixedAsset`

Термопластавтоматы, швейные линии мягкой игрушки, конвейеры покраски, станки ЧПУ.
Привязано к виду выпускаемой игрушки (`ToyCategory`) и возрастной маркировке
(`AgeGroup` — 0+/3+/6+/14+, по ТР ТС 008/2011 «О безопасности игрушек»).

```cpp
enum class ToyCategory { Plastic, Plush, Wooden, BoardGame, Electronic };
enum class AgeGroup    { From0, From3, From6, From14 };   // "0+","3+","6+","14+"

class ProductionEquipment : public FixedAsset {
    std::string model_;
    int capacityPerHour_;     // игрушек/час
    int runHours_;            // фактическая наработка часов
    ToyCategory toyCategory_; // вид выпускаемой игрушки
    AgeGroup    ageGroup_;    // возрастная маркировка

public:
    ProductionEquipment(/* поля базового + */ std::string model,
                        int capacityPerHour, int runHours,
                        ToyCategory toyCategory, AgeGroup ageGroup);

    Money       calculateDepreciation(Date onDate) const override;
    std::string category() const override { return "production"; }
    nlohmann::json toJson() const override;

    // Геттеры/сеттеры специфичных полей
    const auto& model() const { return model_; }
    int capacityPerHour() const { return capacityPerHour_; }
    int runHours() const { return runHours_; }
    void incrementRunHours(int h);   // вызывается из учётной операции
};
```

**Амортизация по наработке:**
```
expectedTotalHours = usefulLifeMonths * 22 * 8    // 22 дня × 8 часов
износ = initialCost * (runHours / expectedTotalHours), но не больше initialCost
```

### `Building : FixedAsset`

Цеха, склады, административные корпуса.

```cpp
class Building : public FixedAsset {
    double area_;           // кв.м
    std::string address_;

public:
    Money       calculateDepreciation(Date onDate) const override;
    std::string category() const override { return "building"; }
    nlohmann::json toJson() const override;
};
```

**Линейная амортизация:**
```
monthsElapsed = monthsBetween(acquiredOn, onDate)
износ = initialCost * (monthsElapsed / usefulLifeMonths), но не больше initialCost
```

### `Vehicle : FixedAsset`

Электропогрузчики, фургоны.

```cpp
class Vehicle : public FixedAsset {
    std::string licensePlate_;
    int mileage_;            // км
    int expectedTotalMileage_;

public:
    Money       calculateDepreciation(Date onDate) const override;
    std::string category() const override { return "vehicle"; }
    nlohmann::json toJson() const override;

    void addMileage(int km);
};
```

**Амортизация по пробегу:**
```
износ = initialCost * (mileage / expectedTotalMileage)
```

### `OfficeEquipment : FixedAsset`

Компьютеры бухгалтерии, принтеры, копиры.

```cpp
class OfficeEquipment : public FixedAsset {
    std::string serialNumber_;

public:
    Money       calculateDepreciation(Date onDate) const override;
    std::string category() const override { return "office"; }
    nlohmann::json toJson() const override;
};
```

**Метод суммы лет (ускоренная):**
```
N = usefulLifeMonths / 12
SumYears = N*(N+1)/2
yearsElapsed = ...
коэффициент_за_год_i = (N - i + 1) / SumYears
суммируем за прошедшие годы
```

---

## Сопутствующие сущности

### `Department`
```cpp
// Цеха игрушечной фабрики (+ Production как общий тип для совместимости)
enum class DepartmentType {
    Production, Molding, SoftToy, Painting, Assembly, Packaging, QualityControl, Warehouse, Admin
};

class Department {
    int id_;
    std::string name_;       // "Цех №1 — литьё"
    std::string head_;       // ФИО начальника
    DepartmentType type_;
public:
    /* геттеры, toJson */
};
```

### `AssetMovement`
Запись в журнале движений (immutable после создания).

```cpp
class AssetMovement {
    int           id_;
    int           assetId_;
    MovementType  type_;
    std::optional<int> fromDepartmentId_;
    std::optional<int> toDepartmentId_;
    Date          date_;
    std::string   note_;
    std::string   performedBy_;
public:
    /* конструктор + геттеры, toJson */
};
```

### `MaintenanceRecord`
```cpp
enum class MaintenanceType { Planned, Emergency };

class MaintenanceRecord {
    int id_;
    Date date_;
    MaintenanceType type_;
    Money cost_;
    std::string description_;
    std::string performedBy_;
public:
    /* конструктор, геттеры, toJson */
};
```

---

## Иерархия исключений

```cpp
namespace domain {

class DomainException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class NotFoundException : public DomainException {
public:
    NotFoundException(const std::string& entity, int id)
        : DomainException(entity + " not found: id=" + std::to_string(id)) {}
};

class ValidationException : public DomainException {
public:
    using DomainException::DomainException;
};

class InvalidStateException : public DomainException {
public:
    using DomainException::DomainException;
};

}
```

Контроллеры в `api/` ловят их и мапят на HTTP-коды:
- `NotFoundException` → 404
- `ValidationException` → 400
- `InvalidStateException` → 409 (Conflict)
- любой другой → 500

---

## Сервисы

### `AssetService`
```cpp
class AssetService {
    std::shared_ptr<IAssetRepository> assetRepo_;
    std::shared_ptr<IDepartmentRepository> deptRepo_;
    MovementService& movements_;
public:
    int  add(std::unique_ptr<FixedAsset> asset);   // возвращает id
    void update(int id, std::unique_ptr<FixedAsset> newData);
    void remove(int id);                            // запретить, если не списан
    void transfer(int id, int newDeptId);          // обновляет + создаёт Movement
    void sendToRepair(int id);
    void backFromRepair(int id, Money repairCost);
    void writeOff(int id, const std::string& reason);

    const FixedAsset& get(int id) const;
    std::vector<const FixedAsset*> list(const AssetFilter& f) const;
};
```

### `MovementService`
```cpp
class MovementService {
public:
    void recordAcquisition(int assetId, int deptId);
    void recordTransfer(int assetId, int fromId, int toId);
    void recordRepair(int assetId, MovementType type);
    void recordWriteOff(int assetId, const std::string& reason);
    std::vector<AssetMovement> history(int assetId) const;
    std::vector<AssetMovement> inPeriod(Date from, Date to) const;
};
```

### `ReportService`
```cpp
class ReportService {
public:
    struct DepreciationRow {
        int    assetId;
        std::string inventoryNo;
        std::string name;
        Money  initialCost;
        Money  accumulated;
        Money  residual;
    };
    std::vector<DepreciationRow> depreciationReport(Date on) const;

    struct DeptSummary {
        int    departmentId;
        std::string departmentName;
        int    assetCount;
        Money  totalResidual;
    };
    std::vector<DeptSummary> byDepartment(Date on) const;
};
```
