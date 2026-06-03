# REST API

Все ответы и тела запросов — JSON. Кодировка UTF-8. Сервер слушает `http://localhost:8080`.

## Конвенции

- Успех: `200 OK` (GET, PUT), `201 Created` (POST), `204 No Content` (DELETE)
- Ошибки: `400` валидация, `404` не найден, `409` конфликт состояния, `500` неожиданное
- Тело ошибки: `{ "error": "...", "details": "..." }`
- Даты — `"YYYY-MM-DD"`, суммы — целое число копеек (`"initialCost": 1500000` = 15 000 ₽)

---

## Основные фонды

### `GET /api/assets`

Список основных фондов с фильтрами.

**Query params:** `category`, `departmentId`, `status`, `q` (поиск по имени/инв.номеру)

**Пример:**
```
GET /api/assets?category=production&status=InUse
```

**Ответ 200:**
```json
[
  {
    "id": 1,
    "inventoryNo": "ТПА-001",
    "name": "Термопластавтомат BOY 50",
    "category": "production",
    "initialCost": 350000000,
    "acquiredOn": "2022-03-15",
    "usefulLifeMonths": 120,
    "status": "InUse",
    "departmentId": 1,
    "model": "BOY 50A",
    "capacityPerHour": 200,
    "runHours": 8400,
    "toyCategory": "plastic",
    "ageGroup": "3+"
  }
]
```

### `GET /api/assets/{id}`

Получить один объект (с историей ТО).

**Ответ 200** — как элемент массива из `/api/assets` плюс поле `maintenance: [...]`.
**Ответ 404** — если не найден.

### `POST /api/assets`

Создать новый фонд. Поле `category` определяет конкретный тип.

**Тело:**
```json
{
  "inventoryNo": "ТПА-002",
  "name": "ТПА Krauss-Maffei",
  "category": "production",
  "initialCost": 520000000,
  "acquiredOn": "2026-05-01",
  "usefulLifeMonths": 144,
  "departmentId": 1,
  "model": "KM160",
  "capacityPerHour": 240,
  "runHours": 0,
  "toyCategory": "plastic",
  "ageGroup": "3+"
}
```

Для `category: "production"` поля `toyCategory` (`plastic`/`plush`/`wooden`/`board`/`electronic`)
и `ageGroup` (`0+`/`3+`/`6+`/`14+`) обязательны.

**Ответ 201:** созданный объект с заполненным `id`.
**Ответ 400:** валидация (дубликат инв.№, недопустимое подразделение, и т.д.).

### `PUT /api/assets/{id}`

Обновить (тело такое же, как POST). Запрещено менять `category` существующего объекта.

### `DELETE /api/assets/{id}`

Удалить. Разрешено **только** если `status == WrittenOff`. Иначе `409 Conflict`.

---

## Операции над фондом

### `POST /api/assets/{id}/transfer`
Переместить в другое подразделение.
```json
{ "toDepartmentId": 3, "note": "Перевод в цех №2" }
```
Создаёт запись `AssetMovement` типа `Transferred`, меняет `departmentId`.

### `POST /api/assets/{id}/send-to-repair`
```json
{ "note": "Замена сопла" }
```
Меняет статус на `InRepair`, создаёт `AssetMovement{SentToRepair}`.

### `POST /api/assets/{id}/back-from-repair`
```json
{ "cost": 4500000, "description": "Замена сопла + калибровка" }
```
Меняет статус на `InUse`, создаёт `MaintenanceRecord` и `AssetMovement{BackFromRepair}`.

### `POST /api/assets/{id}/write-off`
```json
{ "reason": "Полная амортизация, неремонтопригоден" }
```
Меняет статус на `WrittenOff`, создаёт `AssetMovement{WrittenOff}`.

---

## Подразделения

### `GET /api/departments`
```json
[
  { "id": 1, "name": "Цех №1 — литьё", "head": "Иванов И.И.", "type": "Production" },
  { "id": 2, "name": "Цех №2 — покраска", "head": "Петров П.П.", "type": "Production" },
  { "id": 3, "name": "Склад готовой продукции", "head": "Сидоров С.С.", "type": "Warehouse" }
]
```

### `POST /api/departments`
```json
{ "name": "...", "head": "...", "type": "Production" }
```

---

## Движения

### `GET /api/movements`
**Query:** `assetId`, `from` (YYYY-MM-DD), `to` (YYYY-MM-DD), `type`

```json
[
  {
    "id": 12,
    "assetId": 1,
    "type": "Transferred",
    "fromDepartmentId": 1,
    "toDepartmentId": 2,
    "date": "2026-04-10",
    "note": "Перенаправлен на покраску",
    "performedBy": "admin"
  }
]
```

---

## Отчёты

### `GET /api/reports/depreciation?onDate=YYYY-MM-DD&format=json|csv`

Расчёт амортизации на указанную дату. Параметр `format` выбирает стратегию вывода
(`IReportFormatter`): `json` (по умолчанию) или `csv` (выгрузка с заголовком
`Content-Disposition: attachment`, UTF-8 BOM для Excel).

```json
{
  "onDate": "2026-06-01",
  "rows": [
    {
      "assetId": 1,
      "inventoryNo": "ТПА-001",
      "name": "Термопластавтомат BOY 50",
      "category": "production",
      "initialCost": 350000000,
      "accumulated": 142000000,
      "residual": 208000000
    }
  ],
  "totals": {
    "initialCost": 350000000,
    "accumulated": 142000000,
    "residual": 208000000
  }
}
```

### `GET /api/reports/by-department?onDate=YYYY-MM-DD`

Свёртка по подразделениям.

```json
[
  { "departmentId": 1, "departmentName": "Цех №1", "assetCount": 12, "totalResidual": 870000000 },
  { "departmentId": 2, "departmentName": "Цех №2", "assetCount": 7,  "totalResidual": 410000000 }
]
```

### `GET /api/reports/capacity`

Производственные мощности по видам игрушек — свёртка производственного оборудования
по `toyCategory` с суммарной производительностью.

```json
[
  { "toyCategory": "plastic", "lineCount": 3, "totalCapacityPerHour": 940 },
  { "toyCategory": "plush",   "lineCount": 1, "totalCapacityPerHour": 60 },
  { "toyCategory": "wooden",  "lineCount": 1, "totalCapacityPerHour": 40 }
]
```

### `GET /api/reports/movements?from=YYYY-MM-DD&to=YYYY-MM-DD`

Движения за период (то же, что `/api/movements`, но агрегированно по типам в `totals`).

---

## Статика

```
GET /             → web/index.html
GET /assets.html  → web/assets.html
GET /css/*        → web/css/*
GET /js/*         → web/js/*
```

---

## Примеры curl для отладки

```bash
# Список
curl http://localhost:8080/api/assets

# Создание
curl -X POST http://localhost:8080/api/assets \
  -H "Content-Type: application/json" \
  -d '{"inventoryNo":"ТПА-002","name":"...","category":"production",...}'

# Отчёт
curl "http://localhost:8080/api/reports/depreciation?onDate=2026-06-01"
```
