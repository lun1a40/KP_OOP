#include "domain/Department.h"

#include "exceptions/DomainException.h"

namespace domain {

Department::Department(int id, std::string name, std::string head, DepartmentType type)
    : id_(id), name_(std::move(name)), head_(std::move(head)), type_(type) {
    if (name_.empty()) {
        throw ValidationException("Название подразделения не может быть пустым");
    }
}

nlohmann::json Department::toJson() const {
    return {
        {"id",   id_},
        {"name", name_},
        {"head", head_},
        {"type", toString(type_)},
    };
}

}  // namespace domain
