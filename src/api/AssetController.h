#pragma once

#include "httplib.h"
#include "service/AssetService.h"

namespace api {

// REST-контроллер фондов. Только парсинг запроса → вызов сервиса → JSON-ответ.
// Бизнес-правила и валидация — в AssetService.
class AssetController {
public:
    explicit AssetController(service::AssetService& assets);
    void registerRoutes(httplib::Server& srv);

private:
    service::AssetService& assets_;
};

}  // namespace api
