#pragma once

#include "httplib.h"
#include "service/MovementService.h"

namespace api {

class MovementController {
public:
    explicit MovementController(service::MovementService& movements);
    void registerRoutes(httplib::Server& srv);

private:
    service::MovementService& movements_;
};

}  // namespace api
