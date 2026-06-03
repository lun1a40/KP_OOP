#pragma once

#include <memory>
#include <vector>

#include "domain/FixedAsset.h"

namespace repository {

// Абстракция доступа к фондам (паттерн «Репозиторий»).
// Сервисный слой работает только через этот интерфейс — не зная,
// что под ним JSON-файл. Это позволяет подменить хранилище без правок сервисов.
class IAssetRepository {
public:
    virtual ~IAssetRepository() = default;

    // Возвращает указатель на хранимый объект или nullptr, если не найден.
    virtual domain::FixedAsset*       find(int id) = 0;
    virtual const domain::FixedAsset* find(int id) const = 0;

    // Все фонды (указатели на владеемые репозиторием объекты).
    virtual std::vector<domain::FixedAsset*> all() = 0;

    // Вставка нового фонда. Если id == 0 — присваивается следующий свободный.
    // Возвращает итоговый id. Владение передаётся репозиторию.
    virtual int add(std::unique_ptr<domain::FixedAsset> asset) = 0;

    // Удаление по id (бросает NotFoundException, если нет).
    virtual void remove(int id) = 0;

    // Сброс текущего состояния на диск (вызывается после мутаций объектов).
    virtual void persist() = 0;

    // Следующий свободный идентификатор.
    virtual int nextId() const = 0;
};

}  // namespace repository
