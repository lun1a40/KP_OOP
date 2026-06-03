#pragma once

#include <vector>

#include "domain/Department.h"

namespace repository {

// Абстракция доступа к подразделениям.
class IDepartmentRepository {
public:
    virtual ~IDepartmentRepository() = default;

    virtual domain::Department*       find(int id) = 0;
    virtual const domain::Department* find(int id) const = 0;
    virtual std::vector<domain::Department*> all() = 0;
    virtual int  add(domain::Department dept) = 0;   // id присваивается, если 0
    virtual void persist() = 0;
    virtual int  nextId() const = 0;
};

}  // namespace repository
