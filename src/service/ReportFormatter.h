#pragma once

#include <memory>
#include <string>

#include "service/ReportService.h"

namespace service {

// Паттерн «Стратегия»: разные взаимозаменяемые способы вывода одного и того же
// отчёта об амортизации. Контроллер выбирает реализацию по параметру ?format
// и работает с ней через единый интерфейс.
class IReportFormatter {
public:
    virtual ~IReportFormatter() = default;
    virtual std::string contentType() const = 0;
    virtual std::string format(const ReportService::DepreciationReport& report) const = 0;

    // Фабрика стратегии по строковому коду формата ("json" | "csv").
    static std::unique_ptr<IReportFormatter> create(const std::string& format);
};

// Вывод в JSON (формат по умолчанию для REST).
class JsonReportFormatter : public IReportFormatter {
public:
    std::string contentType() const override { return "application/json; charset=utf-8"; }
    std::string format(const ReportService::DepreciationReport& report) const override;
};

// Вывод в CSV (для выгрузки в Excel).
class CsvReportFormatter : public IReportFormatter {
public:
    std::string contentType() const override { return "text/csv; charset=utf-8"; }
    std::string format(const ReportService::DepreciationReport& report) const override;
};

}  // namespace service
