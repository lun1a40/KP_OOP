# Сборка и запуск

Проект собирается тремя способами: Docker (основной), CMake локально и Visual Studio
на Windows. Код портируемый (C++17, `std::filesystem`, без POSIX-only вызовов),
библиотеки header-only лежат в `third_party/` — внешних зависимостей ставить не нужно.

---

## Способ 1 — Docker (рекомендуется)

Требуется только Docker + Docker Compose.

```bash
docker compose up --build      # сборка образа и запуск
docker compose down            # остановка
```

Сервер слушает `http://localhost:8080`. Образ многоступенчатый: на этапе `build`
ставится `g++ + cmake`, бинарь компилируется; в `runtime` копируется только
исполняемый файл + `web/` + `data/`. Каталог `data/` монтируется как volume с хоста,
поэтому изменения фондов сохраняются между запусками.

---

## Способ 2 — CMake локально (Linux / macOS)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/ToyFactoryAssets        # запускать из корня проекта
```

`CMakeLists.txt` сам подхватывает все `.cpp` из `src/` (glob с `CONFIGURE_DEPENDS`),
подключает `src/` и `third_party/` в include-пути и линкуется с потоками.

---

## Способ 3 — Visual Studio (Windows)

### Требования
- **Windows 10/11**, **Visual Studio 2019/2022** (Community), workload
  **Desktop development with C++**.

### Через CMake (проще)
```bat
cmake -S . -B build -G "Visual Studio 17 2022"
```
Открыть `build\ToyFactoryAssets.sln`, собрать (F7), запустить (F5).
В свойствах отладки указать **Working Directory:** `$(SolutionDir)..` — чтобы сервер
находил `web/` и `data/` относительными путями.

### Вручную (без CMake)
Ниже — как создать проект с нуля, если CMake использовать не хочется.

---

## Создание проекта в Visual Studio с нуля

Если SLN уже есть — просто откройте его и собирайте. Ниже — как создать заново.

### 1. New Project
- Тип: **Console App** (Empty Project тоже подойдёт)
- Имя: `ToyFactoryAssets`
- Расположение: корневая папка проекта (где лежат `src/`, `web/`, `data/`)

### 2. Project properties (Configuration: All Configurations, Platform: All Platforms)

**General → C++ Language Standard:** `ISO C++17 Standard (/std:c++17)`

**C/C++ → General → Additional Include Directories:**
```
$(ProjectDir)src;$(ProjectDir)third_party
```

**C/C++ → Command Line → Additional Options:**
```
/utf-8
```
Это критично — без этого MSVC будет считать исходники CP1251 и Cyrillic-литералы сломаются.

**Linker → System → SubSystem:** `Console (/SUBSYSTEM:CONSOLE)`

### 3. Добавить файлы

В Solution Explorer создайте фильтры (виртуальные папки), отражающие реальную структуру:
- `Source Files` → подгруппы `domain`, `repository`, `service`, `api`, `server`, `util`
- `Header Files` → аналогично
- `Resource Files` → пусто

Добавьте через **Add → Existing Item** все `.cpp`/`.h` из `src/`.

### 4. Working directory (для дебага)

**Debugging → Working Directory:** `$(ProjectDir)` — чтобы при `F5` сервер находил `web/`, `data/` относительными путями.

---

## Зависимости

### cpp-httplib
Один файл: `third_party/httplib.h`.

Скачать вручную: https://github.com/yhirose/cpp-httplib/blob/master/httplib.h

В `main.cpp` достаточно:
```cpp
#include "httplib.h"
```

Никакой линковки. На Windows автоматически линкуется с `Ws2_32.lib` через `#pragma comment(lib, ...)` внутри самого header.

### nlohmann/json
Один файл: `third_party/nlohmann/json.hpp`.

Скачать: https://github.com/nlohmann/json/releases — `json.hpp` из последнего релиза.

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;
```

---

## Подготовка данных

При первом запуске должны существовать (хотя бы пустые) файлы:

`data/departments.json`:
```json
[
  { "id": 1, "name": "Цех №1 — литьё", "head": "Иванов И.И.", "type": "Production" }
]
```

`data/assets.json`:
```json
[]
```

`data/movements.json`:
```json
[]
```

Если файла нет — репозиторий должен его создать пустым при первом сохранении.

---

## Запуск

1. **F5** в Visual Studio
2. В консоли увидите `Server listening on http://localhost:8080`
3. Откройте `http://localhost:8080` в браузере

### Если порт занят
Поменять в `main.cpp`: `server.listen("0.0.0.0", 8081)`.

### Если кириллица в консоли — кракозябры
Это **только консольный вывод**, файлы сохраняются корректно. Поправить можно вызовом `SetConsoleOutputCP(CP_UTF8)` в начале `main`.

---

## Чек-лист перед сдачей

- [ ] Проект собирается на чистой машине без warning-ов (`/W3`)
- [ ] При запуске данные подгружаются и сохраняются
- [ ] Все CRUD-операции работают через UI
- [ ] Расчёт амортизации проверен на 3-х датах (вчера, сегодня, далеко в будущем — должно быть = initialCost)
- [ ] Журнал движений содержит правильные записи при каждой операции
- [ ] При удалении не-списанного фонда — ошибка (409)
- [ ] Записка содержит UML-диаграмму классов
- [ ] В записке отмечены все ООП-демонстрации
