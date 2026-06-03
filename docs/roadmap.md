# План реализации

Каждый шаг должен заканчиваться **компилируемым и работающим** состоянием. Не ломать прошлые этапы.

## Этап 1 — Скелет (½ дня)

- [ ] Создать VS-проект по [docs/build.md](build.md)
- [ ] Скачать `httplib.h` и `nlohmann/json.hpp` в `third_party/`
- [ ] Написать минимальный `main.cpp`:
  ```cpp
  httplib::Server srv;
  srv.Get("/api/ping", [](auto&, auto& res){ res.set_content("pong", "text/plain"); });
  srv.set_mount_point("/", "./web");
  srv.listen("0.0.0.0", 8080);
  ```
- [ ] Положить `web/index.html` с надписью «Hello», открыть в браузере
- [ ] **Проверка:** `curl http://localhost:8080/api/ping` → `pong`, `http://localhost:8080` показывает HTML

## Этап 2 — Утилиты (½ дня)

- [ ] `util/Date.h/.cpp` с тестами вручную в `main` (вывести `Date::today()`, разность дат)
- [ ] `util/Money.h/.cpp`
- [ ] `domain/AssetStatus.h`, `domain/MovementType.h`
- [ ] `exceptions/*.h` — иерархия исключений

## Этап 3 — Доменное ядро (1 день)

- [ ] `FixedAsset` — абстрактный, с базовыми полями, виртуальными `calculateDepreciation`/`category`/`toJson`
- [ ] `ProductionEquipment` — наследник, с амортизацией по наработке
- [ ] **В `main` временно создать вручную:** один `unique_ptr<ProductionEquipment>`, вывести `toJson()` в консоль, посчитать амортизацию
- [ ] `Building`, `Vehicle`, `OfficeEquipment` — по очереди
- [ ] `MaintenanceRecord`, `AssetMovement`, `Department`

**Зачем сначала вручную:** убедиться, что полиморфизм работает, прежде чем городить репозитории.

## Этап 4 — Репозиторий (½ дня)

- [ ] `IAssetRepository` — чистый интерфейс
- [ ] `JsonMapper::assetFromJson` — фабрика, switch по `category`
- [ ] `JsonAssetRepository` — load/save/findById/findAll/remove
- [ ] **Проверка:** в main создать пару фондов через репозиторий, перезапустить программу, убедиться что грузятся обратно
- [ ] То же для `Department`, `Movement`

## Этап 5 — Сервисный слой (½ дня)

- [ ] `AssetService` с операциями `add/update/remove/get/list`, валидация дубликатов инв.№
- [ ] `MovementService` (создаёт записи при операциях)
- [ ] Связать: `AssetService::add` вызывает `MovementService::recordAcquisition`

## Этап 6 — REST API (1 день)

- [ ] `AssetController` → `GET /api/assets`, `GET /:id`, `POST`, `PUT`, `DELETE`
- [ ] Конвертация ошибок: `try/catch (NotFoundException) → 404`, и т.д.
- [ ] `DepartmentController`, `MovementController`
- [ ] Эндпоинты для `transfer`, `write-off` и т.д.
- [ ] **Проверка через `curl` или Postman** до того, как делать фронт

## Этап 7 — Фронтенд: основная страница (1 день)

- [ ] `web/index.html` — дашборд (счётчики)
- [ ] `web/assets.html` — таблица фондов с фильтрами
- [ ] `web/js/api.js` — обёртка `fetch`
- [ ] Модалка добавления фонда: при выборе `category` показываются нужные доп. поля
- [ ] Кнопки операций (Перевести, На ремонт, Списать)

## Этап 8 — Отчёты (½ дня)

- [ ] `ReportService::depreciationReport`
- [ ] `ReportService::byDepartment`
- [ ] `ReportController` → `/api/reports/...`
- [ ] `web/reports.html` — таблица + выбор даты

## Этап 9 — Полировка (½ дня)

- [ ] Стили (`web/css/styles.css`) — не дизайн уровня production, но опрятно
- [ ] Валидация форм на фронте + понятные сообщения об ошибках
- [ ] Несколько демо-данных в `data/assets.json` (5-10 фондов разных типов)
- [ ] Проверить, что всё работает с чистой машины (удалить bin/obj, пересобрать)

## Этап 10 — Записка (1-2 дня)

- [ ] Введение, постановка задачи
- [ ] **Диаграмма классов** (UML) — нарисовать в draw.io / Visual Paradigm
- [ ] Описание каждого ООП-принципа с указанием **конкретного класса/метода** в коде
- [ ] Описание REST API
- [ ] Скриншоты UI с пояснениями
- [ ] Заключение, листинги (по требованию методички)

---

## Итого

≈ 6-7 рабочих дней, не считая записку. Если что-то затягивается — режется этап 9 (полировка), не этап 6 (API) и не записка.
