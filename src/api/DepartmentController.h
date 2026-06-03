#pragma once

#include "httplib.h"
#include "service/DepartmentService.h"

namespace api {

class DepartmentController {
public:
    explicit DepartmentController(service::DepartmentService& departments);
    void registerRoutes(httplib::Server& srv);

private:
    service::DepartmentService& departments_;
};

}  // namespace api
