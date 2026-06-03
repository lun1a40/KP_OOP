#include "api/AssetController.h"

#include "api/HttpJson.h"
#include "repository/JsonMapper.h"
#include "service/AssetFilter.h"
#include "domain/AssetStatus.h"

namespace api {

using service::AssetFilter;
using repository::JsonMapper;

AssetController::AssetController(service::AssetService& assets) : assets_(assets) {}

void AssetController::registerRoutes(httplib::Server& srv) {
    // GET /api/assets — список с фильтрами
    srv.Get("/api/assets", [this](const httplib::Request& req, httplib::Response& res) {
        AssetFilter filter;
        if (req.has_param("category"))     filter.category = req.get_param_value("category");
        if (req.has_param("departmentId")) filter.departmentId = std::stoi(req.get_param_value("departmentId"));
        if (req.has_param("status"))       filter.status = domain::assetStatusFromString(req.get_param_value("status"));
        if (req.has_param("q"))            filter.query = req.get_param_value("q");

        nlohmann::json arr = nlohmann::json::array();
        for (const auto* a : assets_.list(filter)) {
            arr.push_back(a->toJson());
        }
        sendJson(res, arr);
    });

    // GET /api/assets/{id} — один фонд с историей ТО
    srv.Get(R"(/api/assets/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        sendJson(res, assets_.get(pathInt(req, 1)).toJson());
    });

    // POST /api/assets — создать
    srv.Post("/api/assets", [this](const httplib::Request& req, httplib::Response& res) {
        auto asset = JsonMapper::assetFromJson(parseBody(req));
        asset->setId(0);                       // id присваивает сервис/репозиторий
        const int id = assets_.add(std::move(asset));
        sendJson(res, assets_.get(id).toJson(), 201);
    });

    // PUT /api/assets/{id} — обновить
    srv.Put(R"(/api/assets/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        const int id = pathInt(req, 1);
        assets_.update(id, JsonMapper::assetFromJson(parseBody(req)));
        sendJson(res, assets_.get(id).toJson());
    });

    // DELETE /api/assets/{id} — удалить (только списанный)
    srv.Delete(R"(/api/assets/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        assets_.remove(pathInt(req, 1));
        res.status = 204;
    });

    // POST /api/assets/{id}/transfer
    srv.Post(R"(/api/assets/(\d+)/transfer)", [this](const httplib::Request& req, httplib::Response& res) {
        const int id = pathInt(req, 1);
        const auto body = parseBody(req);
        assets_.transfer(id, body.at("toDepartmentId").get<int>(), body.value("note", std::string{}));
        sendJson(res, assets_.get(id).toJson());
    });

    // POST /api/assets/{id}/send-to-repair
    srv.Post(R"(/api/assets/(\d+)/send-to-repair)", [this](const httplib::Request& req, httplib::Response& res) {
        const int id = pathInt(req, 1);
        const auto body = parseBody(req);
        assets_.sendToRepair(id, body.value("note", std::string{}));
        sendJson(res, assets_.get(id).toJson());
    });

    // POST /api/assets/{id}/back-from-repair
    srv.Post(R"(/api/assets/(\d+)/back-from-repair)", [this](const httplib::Request& req, httplib::Response& res) {
        const int id = pathInt(req, 1);
        const auto body = parseBody(req);
        assets_.backFromRepair(id, util::Money(body.at("cost").get<std::int64_t>()),
                               body.value("description", std::string{}));
        sendJson(res, assets_.get(id).toJson());
    });

    // POST /api/assets/{id}/write-off
    srv.Post(R"(/api/assets/(\d+)/write-off)", [this](const httplib::Request& req, httplib::Response& res) {
        const int id = pathInt(req, 1);
        const auto body = parseBody(req);
        assets_.writeOff(id, body.value("reason", std::string{}));
        sendJson(res, assets_.get(id).toJson());
    });
}

}  // namespace api
