#include "service/DepartmentService.h"

#include "exceptions/DomainException.h"

namespace service {

DepartmentService::DepartmentService(repository::IDepartmentRepository& repo) : repo_(repo) {}

std::vector<domain::Department*> DepartmentService::list() {
    return repo_.all();
}

const domain::Department& DepartmentService::get(int id) {
    const auto* d = repo_.find(id);
    if (!d) throw domain::NotFoundException("Department", id);
    return *d;
}

int DepartmentService::add(domain::Department dept) {
    return repo_.add(std::move(dept));
}

}  // namespace service
