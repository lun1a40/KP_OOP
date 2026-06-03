#include "repository/JsonDepartmentRepository.h"

#include "repository/JsonFile.h"
#include "repository/JsonMapper.h"

namespace repository {

JsonDepartmentRepository::JsonDepartmentRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    load();
}

void JsonDepartmentRepository::load() {
    departments_.clear();
    const nlohmann::json arr = readArray(filePath_);
    for (const auto& item : arr) {
        domain::Department dept = JsonMapper::departmentFromJson(item);
        departments_.emplace(dept.id(), std::move(dept));
    }
}

domain::Department* JsonDepartmentRepository::find(int id) {
    auto it = departments_.find(id);
    return it == departments_.end() ? nullptr : &it->second;
}

const domain::Department* JsonDepartmentRepository::find(int id) const {
    auto it = departments_.find(id);
    return it == departments_.end() ? nullptr : &it->second;
}

std::vector<domain::Department*> JsonDepartmentRepository::all() {
    std::vector<domain::Department*> result;
    result.reserve(departments_.size());
    for (auto& [id, dept] : departments_) {
        result.push_back(&dept);
    }
    return result;
}

int JsonDepartmentRepository::add(domain::Department dept) {
    int id = dept.id();
    if (id == 0) {
        id = nextId();
        dept.setId(id);
    }
    departments_.emplace(id, std::move(dept));
    persist();
    return id;
}

void JsonDepartmentRepository::persist() {
    nlohmann::json arr = nlohmann::json::array();
    for (auto& [id, dept] : departments_) {
        arr.push_back(dept.toJson());
    }
    writeJson(filePath_, arr);
}

int JsonDepartmentRepository::nextId() const {
    return departments_.empty() ? 1 : departments_.rbegin()->first + 1;
}

}  // namespace repository
