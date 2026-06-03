#include "service/ReportFormatter.h"

#include <sstream>

#include "nlohmann/json.hpp"

namespace service {

std::unique_ptr<IReportFormatter> IReportFormatter::create(const std::string& format) {
    if (format == "csv") {
        return std::make_unique<CsvReportFormatter>();
    }
    return std::make_unique<JsonReportFormatter>();  // по умолчанию
}

std::string JsonReportFormatter::format(const ReportService::DepreciationReport& report) const {
    nlohmann::json rows = nlohmann::json::array();
    for (const auto& r : report.rows) {
        rows.push_back({
            {"assetId",     r.assetId},
            {"inventoryNo", r.inventoryNo},
            {"name",        r.name},
            {"category",    r.category},
            {"initialCost", r.initialCost.kopecks()},
            {"accumulated", r.accumulated.kopecks()},
            {"residual",    r.residual.kopecks()},
        });
    }
    const nlohmann::json out = {
        {"onDate", report.onDate.toIso()},
        {"rows",   rows},
        {"totals", {
            {"initialCost", report.totalInitial.kopecks()},
            {"accumulated", report.totalAccumulated.kopecks()},
            {"residual",    report.totalResidual.kopecks()},
        }},
    };
    return out.dump();
}

namespace {

// Рубли с точкой-разделителем — нейтрально для парсеров CSV.
std::string rubles(util::Money m) {
    std::int64_t k = m.kopecks();
    const bool neg = k < 0;
    if (neg) k = -k;
    std::ostringstream os;
    if (neg) os << '-';
    os << (k / 100) << '.' << (k % 100 < 10 ? "0" : "") << (k % 100);
    return os.str();
}

// Экранирование поля CSV (кавычки удваиваются, поле берётся в кавычки при нужде).
std::string csvField(const std::string& s) {
    if (s.find(';') == std::string::npos && s.find('"') == std::string::npos &&
        s.find('\n') == std::string::npos) {
        return s;
    }
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += '"';
    return out;
}

}  // namespace

std::string CsvReportFormatter::format(const ReportService::DepreciationReport& report) const {
    std::ostringstream os;
    os << "\xEF\xBB\xBF";  // UTF-8 BOM — чтобы Excel распознал кириллицу
    os << "inventoryNo;name;category;initialCost;accumulated;residual\r\n";
    for (const auto& r : report.rows) {
        os << csvField(r.inventoryNo) << ';'
           << csvField(r.name) << ';'
           << csvField(r.category) << ';'
           << rubles(r.initialCost) << ';'
           << rubles(r.accumulated) << ';'
           << rubles(r.residual) << "\r\n";
    }
    os << ";;ИТОГО;"
       << rubles(report.totalInitial) << ';'
       << rubles(report.totalAccumulated) << ';'
       << rubles(report.totalResidual) << "\r\n";
    return os.str();
}

}  // namespace service
