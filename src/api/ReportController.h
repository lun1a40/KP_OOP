#pragma once

#include "httplib.h"
#include "service/ReportService.h"
#include "service/MovementService.h"

namespace api {

class ReportController {
public:
    ReportController(service::ReportService& reports, service::MovementService& movements);
    void registerRoutes(httplib::Server& srv);

private:
    service::ReportService&   reports_;
    service::MovementService& movements_;
};

}  // namespace api
