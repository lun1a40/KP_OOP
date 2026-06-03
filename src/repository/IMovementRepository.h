#pragma once

#include <vector>

#include "domain/AssetMovement.h"

namespace repository {

// Абстракция доступа к журналу движений. Журнал append-only:
// записи добавляются, но не изменяются и не удаляются.
class IMovementRepository {
public:
    virtual ~IMovementRepository() = default;

    virtual std::vector<const domain::AssetMovement*> all() const = 0;
    virtual int  add(domain::AssetMovement movement) = 0;   // id присваивается, если 0
    virtual void persist() = 0;
    virtual int  nextId() const = 0;
};

}  // namespace repository
