#include "api/DepartmentController.h"

#include "api/HttpJson.h"
#include "repository/JsonMapper.h"

namespace api {

DepartmentController::DepartmentController(service::DepartmentService& departments)
    : departments_(departments) {}

void DepartmentController::registerRoutes(httplib::Server& srv) {
    // GET /api/departments
    srv.Get("/api/departments", [this](const httplib::Request&, httplib::Response& res) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto* d : departments_.list()) {
            arr.push_back(d->toJson());
        }
        sendJson(res, arr);
    });

    // POST /api/departments
    srv.Post("/api/departments", [this](const httplib::Request& req, httplib::Response& res) {
        auto dept = repository::JsonMapper::departmentFromJson(parseBody(req));
        dept.setId(0);
        const int id = departments_.add(std::move(dept));
        sendJson(res, departments_.get(id).toJson(), 201);
    });
}

}  // namespace api
