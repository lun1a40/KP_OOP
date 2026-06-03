#pragma once

#include <stdexcept>
#include <string>

namespace domain {

// Базовая ошибка предметной области. Контроллеры в api/ ловят её
// и мапят конкретные подтипы на HTTP-коды.
class DomainException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Сущность не найдена → HTTP 404.
class NotFoundException : public DomainException {
public:
    NotFoundException(const std::string& entity, int id)
        : DomainException(entity + " not found: id=" + std::to_string(id)) {}
    explicit NotFoundException(const std::string& message)
        : DomainException(message) {}
};

// Невалидные входные данные → HTTP 400.
class ValidationException : public DomainException {
public:
    using DomainException::DomainException;
};

// Операция недопустима в текущем состоянии → HTTP 409.
class InvalidStateException : public DomainException {
public:
    using DomainException::DomainException;
};

}  // namespace domain
