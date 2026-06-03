#include "api/ReportController.h"

#include <map>

#include "api/HttpJson.h"
#include "service/ReportFormatter.h"
#include "domain/MovementType.h"
#include "util/Date.h"

namespace api {

namespace {

// onDate из query или сегодня по умолчанию.
util::Date dateParam(const httplib::Request& req, const std::string& name) {
    if (req.has_param(name)) {
        return util::Date::fromIso(req.get_param_value(name));
    }
    return util::Date::today();
}

}  // namespace

ReportController::ReportController(service::ReportService& reports,
                                  service::MovementService& movements)
    : reports_(reports), movements_(movements) {}

void ReportController::registerRoutes(httplib::Server& srv) {
    // GET /api/reports/depreciation?onDate=YYYY-MM-DD&format=json|csv
    // Формат вывода выбирается стратегией IReportFormatter (паттерн «Стратегия»).
    srv.Get("/api/reports/depreciation", [this](const httplib::Request& req, httplib::Response& res) {
        const auto report = reports_.depreciationReport(dateParam(req, "onDate"));

        const std::string fmt = req.has_param("format") ? req.get_param_value("format") : "json";
        auto formatter = service::IReportFormatter::create(fmt);

        res.status = 200;
        res.set_content(formatter->format(report), formatter->contentType());
        if (fmt == "csv") {
            res.set_header("Content-Disposition", "attachment; filename=\"depreciation.csv\"");
        }
    });

    // GET /api/reports/by-department?onDate=YYYY-MM-DD
    srv.Get("/api/reports/by-department", [this](const httplib::Request& req, httplib::Response& res) {
        const auto summaries = reports_.byDepartment(dateParam(req, "onDate"));
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& s : summaries) {
            arr.push_back({
                {"departmentId",   s.departmentId},
                {"departmentName", s.departmentName},
                {"assetCount",     s.assetCount},
                {"totalResidual",  s.totalResidual.kopecks()},
            });
        }
        sendJson(res, arr);
    });

    // GET /api/reports/capacity — производственные мощности по видам игрушек
    srv.Get("/api/reports/capacity", [this](const httplib::Request&, httplib::Response& res) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& row : reports_.capacityByToyCategory()) {
            arr.push_back({
                {"toyCategory",          row.toyCategory},
                {"lineCount",            row.lineCount},
                {"totalCapacityPerHour", row.totalCapacityPerHour},
            });
        }
        sendJson(res, arr);
    });

    // GET /api/reports/movements?from=YYYY-MM-DD&to=YYYY-MM-DD
    srv.Get("/api/reports/movements", [this](const httplib::Request& req, httplib::Response& res) {
        const util::Date from = dateParam(req, "from");
        const util::Date to   = dateParam(req, "to");

        std::map<std::string, int> totals;
        nlohmann::json items = nlohmann::json::array();
        for (const auto* m : movements_.inPeriod(from, to)) {
            items.push_back(m->toJson());
            totals[domain::toString(m->type())] += 1;
        }
        sendJson(res, {
            {"from",      from.toIso()},
            {"to",        to.toIso()},
            {"totals",    totals},
            {"movements", items},
        });
    });
}

}  // namespace api
