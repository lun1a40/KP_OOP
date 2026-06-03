// ToyFactoryAssets — точка входа.
// Собирает граф зависимостей (ручное DI): репозитории → сервисы → контроллеры,
// затем поднимает HTTP-сервер.

#include <iostream>

#include "repository/JsonAssetRepository.h"
#include "repository/JsonDepartmentRepository.h"
#include "repository/JsonMovementRepository.h"
#include "service/AssetService.h"
#include "service/DepartmentService.h"
#include "service/MovementService.h"
#include "service/ReportService.h"
#include "api/AssetController.h"
#include "api/DepartmentController.h"
#include "api/MovementController.h"
#include "api/ReportController.h"
#include "server/HttpServer.h"

#if defined(_WIN32)
#include <windows.h>
#endif

int main() {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);   // корректный вывод кириллицы в консоль Windows
#endif

    try {
        // 1. Репозитории (хранилище — JSON-файлы в data/)
        repository::JsonAssetRepository      assetRepo("data/assets.json");
        repository::JsonDepartmentRepository deptRepo("data/departments.json");
        repository::JsonMovementRepository   moveRepo("data/movements.json");

        // 2. Сервисы
        service::MovementService   movements(moveRepo);
        service::AssetService      assets(assetRepo, deptRepo, movements);
        service::DepartmentService departments(deptRepo);
        service::ReportService     reports(assetRepo, deptRepo);

        // 3. Сервер + контроллеры
        server::HttpServer server("./web");

        api::AssetController      assetController(assets);
        api::DepartmentController departmentController(departments);
        api::MovementController   movementController(movements);
        api::ReportController     reportController(reports, movements);

        assetController.registerRoutes(server.raw());
        departmentController.registerRoutes(server.raw());
        movementController.registerRoutes(server.raw());
        reportController.registerRoutes(server.raw());

        // health-check
        server.raw().Get("/api/ping", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("pong", "text/plain");
        });

        if (!server.listen("0.0.0.0", 8080)) {
            std::cerr << "FATAL: не удалось занять порт 8080\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "FATAL при старте: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
