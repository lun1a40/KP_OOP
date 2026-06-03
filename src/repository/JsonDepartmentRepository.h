#pragma once

#include <map>
#include <string>

#include "repository/IDepartmentRepository.h"

namespace repository {

// Реализация IDepartmentRepository поверх JSON-файла.
class JsonDepartmentRepository : public IDepartmentRepository {
public:
    explicit JsonDepartmentRepository(std::string filePath);

    domain::Department*       find(int id) override;
    const domain::Department* find(int id) const override;
    std::vector<domain::Department*> all() override;
    int  add(domain::Department dept) override;
    void persist() override;
    int  nextId() const override;

private:
    void load();

    std::string filePath_;
    std::map<int, domain::Department> departments_;
};

}  // namespace repository
