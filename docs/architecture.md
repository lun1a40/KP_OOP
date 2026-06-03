# Архитектура

## Общая идея

Один C++ бинарь, который:
1. Поднимает HTTP-сервер (cpp-httplib) на `localhost:8080`
2. Отдаёт REST API (`/api/...`) и статику фронта (`/`, `/css/...`, `/js/...`)
3. Загружает данные из `data/*.json` при старте, пишет обратно при изменениях

Это сильно упрощает запуск и защиту — никаких отдельных сервисов, БД, рантаймов.

## Слоистая архитектура

```
┌───────────────────────────────────────────────────────────┐
│  web/  (HTML/CSS/JS — формы, таблицы, графики)           │
└──────────────────────────┬────────────────────────────────┘
                           │ HTTP + JSON
┌──────────────────────────▼────────────────────────────────┐
│  src/api/  (REST контроллеры)                             │
│  AssetController, MovementController, ReportController    │
│  JsonMapper — конвертация JSON ↔ доменные объекты         │
└──────────────────────────┬────────────────────────────────┘
                           │ вызовы методов
┌──────────────────────────▼────────────────────────────────┐
│  src/service/  (бизнес-логика)                            │
│  AssetService, MovementService, ReportService             │
│  Транзакционные операции, валидация на уровне процесса    │
└──────────────────────────┬────────────────────────────────┘
                           │
┌──────────────────────────▼────────────────────────────────┐
│  src/repository/  (доступ к данным)                       │
│  IAssetRepository — интерфейс                             │
│  JsonAssetRepository — реализация над JSON-файлом         │
└──────────────────────────┬────────────────────────────────┘
                           │
┌──────────────────────────▼────────────────────────────────┐
│  src/domain/  (ядро ООП)                                  │
│  FixedAsset (абстрактный) и 4 наследника                  │
│  Department, AssetMovement, MaintenanceRecord             │
└───────────────────────────────────────────────────────────┘
```

Каждый слой зависит только от того, что ниже его. Это ключевое: при защите можно показать, что доменный слой не знает ни про JSON, ни про HTTP — чистая модель.

## Диаграмма классов (ядро домена)

```
                     ┌──────────────────────────┐
                     │   <<abstract>>           │
                     │   FixedAsset             │
                     ├──────────────────────────┤
                     │ -id: int                 │
                     │ -inventoryNo: string     │
                     │ -name: string            │
                     │ -initialCost: Money      │
                     │ -acquiredOn: Date        │
                     │ -usefulLifeMonths: int   │
                     │ -status: AssetStatus     │
                     │ -departmentId: int       │
                     │ -maintenance:            │
                     │   vector<Maintenance...> │
                     ├──────────────────────────┤
                     │ +virtual calculate-      │
                     │   Depreciation(on):Money │
                     │ +virtual category():str  │
                     │ +virtual toJson():json   │
                     │ +virtual ~FixedAsset()   │
                     └──────────┬───────────────┘
                                │ наследование
        ┌───────────────────────┼─────────────────────┬─────────────────────┐
        ▼                       ▼                     ▼                     ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│ProductionEquipm. │ │ Building         │ │ Vehicle          │ │ OfficeEquipment  │
├──────────────────┤ ├──────────────────┤ ├──────────────────┤ ├──────────────────┤
│ -model           │ │ -area: double    │ │ -plate: string   │ │ -serialNo        │
│ -capacityPerHour │ │ -address         │ │ -mileage         │ │                  │
│ -runHours        │ │                  │ │ -expectedMileage │ │                  │
├──────────────────┤ ├──────────────────┤ ├──────────────────┤ ├──────────────────┤
│ амортизация:    │ │ амортизация:    │ │ амортизация:    │ │ ускоренная       │
│ по наработке    │ │ линейная (долго) │ │ по пробегу       │ │ (метод суммы лет)│
└──────────────────┘ └──────────────────┘ └──────────────────┘ └──────────────────┘

         композиция              композиция
┌──────────────────┐ ────────► ┌──────────────────┐
│ FixedAsset       │            │ MaintenanceRecord│
│  (history)       │            │ date, cost, type │
└──────────────────┘            └──────────────────┘

         ассоциация
┌──────────────────┐ ────────► ┌──────────────────┐
│ FixedAsset       │            │ Department       │
│  (departmentId)  │            │  id, name, type  │
└──────────────────┘            └──────────────────┘

      агрегация (журнал)
┌──────────────────┐ ◄──────── ┌──────────────────┐
│ FixedAsset       │            │ AssetMovement    │
│                  │            │ assetId, type,   │
│                  │            │ from, to, date   │
└──────────────────┘            └──────────────────┘
```

## Диаграмма классов (инфраструктура)

```
        <<interface>>
     ┌──────────────────────────┐
     │ IAssetRepository         │
     ├──────────────────────────┤
     │ + findById(id)           │
     │ + findAll(filter)        │
     │ + save(asset)            │
     │ + remove(id)             │
     └──────────┬───────────────┘
                │ реализует
                ▼
     ┌──────────────────────────┐         ┌──────────────────┐
     │ JsonAssetRepository      │────────►│ JsonMapper       │
     │ - assets: unordered_map  │         │ (фабрика по      │
     │ - filePath               │         │  полю category)  │
     │ + load(), + persist()    │         └──────────────────┘
     └──────────────────────────┘

     <<exception>>
     ┌──────────────────────────┐
     │ DomainException          │ : std::runtime_error
     └──────────┬───────────────┘
                │
        ┌───────┴───────┐
        ▼               ▼
  ┌──────────────┐ ┌──────────────────┐
  │ NotFound     │ │ ValidationError  │
  └──────────────┘ └──────────────────┘
```

## Поток одного запроса

Пример: пользователь нажимает «Сохранить» в форме добавления оборудования.

```
1. Browser: POST /api/assets   { category:"production", model:"ТПА-160", ... }
                ↓
2. HttpServer (server/HttpServer.cpp)
   маршрутизация → AssetController::create
                ↓
3. AssetController::create
   - JsonMapper::assetFromJson(body)  ← фабричный метод по "category"
                                         создаёт unique_ptr<ProductionEquipment>
   - assetService.add(std::move(asset))
                ↓
4. AssetService::add
   - валидация (уникальность инв.№, корректность дат, и т.д.)
   - assetRepo.save(std::move(asset))
   - movementService.recordAcquisition(asset.id())   ← запись «поступление» в журнал
                ↓
5. JsonAssetRepository::save
   - кладёт в unordered_map
   - persist() — сериализует map в JSON и пишет в файл
                ↓
6. Возврат вверх по стеку → AssetController формирует 201 Created + JSON
```

Любой слой может бросить `DomainException`. Контроллер ловит её и превращает в `400 Bad Request` или `404 Not Found`.

## Инициализация (main.cpp)

Главная функция собирает граф зависимостей (ручное DI):

```cpp
int main() {
    // 1. Репозитории
    auto assetRepo = std::make_shared<JsonAssetRepository>("data/assets.json");
    auto deptRepo  = std::make_shared<JsonDepartmentRepository>("data/departments.json");
    auto moveRepo  = std::make_shared<JsonMovementRepository>("data/movements.json");

    // 2. Сервисы
    AssetService     assets(assetRepo, deptRepo);
    MovementService  movements(moveRepo, assetRepo);
    ReportService    reports(assetRepo, moveRepo);

    // 3. Сервер
    HttpServer server("./web");
    server.registerRoutes(assets, movements, reports);
    server.listen("0.0.0.0", 8080);
}
```

## Конкретно про ООП

Ключевое:

- **Виртуальный `calculateDepreciation`** — самое яркое место полиморфизма. Каждый тип фонда считает амортизацию по-своему, но `ReportService` итерируется по `vector<unique_ptr<FixedAsset>>` единообразно.
- **Виртуальный `toJson`** — каждый наследник добавляет свои поля. Базовый класс пишет общие поля, наследник вызывает `FixedAsset::toJson()` и дополняет.
- **`JsonMapper::assetFromJson`** — фабричный метод. Смотрит на поле `category` и создаёт нужный наследник. Это место, которое нужно расширять при добавлении нового типа фонда.
