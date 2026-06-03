#pragma once

#include <vector>

#include "repository/IDepartmentRepository.h"
#include "domain/Department.h"

namespace service {

// Подразделения — справочные данные без сложной бизнес-логики.
// Сервис сохраняет слоистость: контроллеры обращаются к нему, а не к репозиторию.
class DepartmentService {
public:
    explicit DepartmentService(repository::IDepartmentRepository& repo);

    std::vector<domain::Department*> list();
    const domain::Department&        get(int id);
    int                              add(domain::Department dept);

private:
    repository::IDepartmentRepository& repo_;
};

}  // namespace service
