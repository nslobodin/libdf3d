#include "EntityManager.h"

namespace df3d { namespace game_impl {

EntityManager::EntityManager()
    : m_bag(std::numeric_limits<Entity::IdType>::max() - 1)
{

}

EntityManager::~EntityManager()
{

}

Entity EntityManager::create()
{
    Entity e;
    e.id = m_bag.getNew();

    m_entities.insert(e);

    return e;
}

void EntityManager::destroy(Entity e)
{
    assert(e.valid());

    auto count = m_entities.erase(e);
    assert(count == 1);
    // NOTE: component data should be destroyed later via World::cleanStep

    m_bag.release(e.id);
}

bool EntityManager::alive(Entity e) const
{
    assert(e.valid());

    return m_entities.find(e) != m_entities.end();
}

size_t EntityManager::size() const
{
    return m_entities.size();
}

void EntityManager::cleanStep()
{
    m_bag.cleanup();
}

} }
