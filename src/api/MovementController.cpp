#include "api/MovementController.h"

#include "api/HttpJson.h"
#include "domain/MovementType.h"
#include "util/Date.h"

namespace api {

MovementController::MovementController(service::MovementService& movements)
    : movements_(movements) {}

void MovementController::registerRoutes(httplib::Server& srv) {
    // GET /api/movements?assetId=&from=&to=&type=
    srv.Get("/api/movements", [this](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto* m : movements_.all()) {
            if (req.has_param("assetId") && m->assetId() != std::stoi(req.get_param_value("assetId"))) {
                continue;
            }
            if (req.has_param("type") &&
                domain::toString(m->type()) != req.get_param_value("type")) {
                continue;
            }
            if (req.has_param("from") &&
                m->date() < util::Date::fromIso(req.get_param_value("from"))) {
                continue;
            }
            if (req.has_param("to") &&
                m->date() > util::Date::fromIso(req.get_param_value("to"))) {
                continue;
            }
            arr.push_back(m->toJson());
        }
        sendJson(res, arr);
    });
}

}  // namespace api
