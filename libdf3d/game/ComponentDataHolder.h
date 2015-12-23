#pragma once

#include "Entity.h"

namespace df3d {

// TODO_ecs : add test suits.
// TODO_ecs: more efficient implementation.
template<typename T>
class ComponentDataHolder : utils::NonCopyable
{
public:
    using DestructionCallback = std::function<void(const T&)>;

private:
    std::vector<T> m_data;
    // TODO_ecs: replace with array.
    // Maintain a bag of entities ids. If < 1000 for example, use array, instead - hashmap.
    std::unordered_map<Entity::IdType, ComponentInstance> m_lookup;
    std::unordered_map<ComponentInstance::IdType, Entity> m_holders;

    DestructionCallback m_destructionCallback;

public:
    ComponentDataHolder()
    {

    }

    ~ComponentDataHolder()
    {
        clear();
    }

    ComponentInstance lookup(Entity e)
    {
        assert(e.valid());

        auto found = m_lookup.find(e.id);
        if (found == m_lookup.end())
            return ComponentInstance();

        return found->second;
    }

    void add(Entity e, const T &componentData)
    {
        assert(e.valid());
        assert(!contains(e));

        ComponentInstance inst(m_data.size());
        m_data.push_back(componentData);
        m_lookup[e.id] = inst;

        m_holders[inst.id] = e;
    }

    void remove(Entity e)
    {
        assert(e.valid());
        assert(m_data.size() == m_lookup.size() && m_holders.size() == m_lookup.size());
        assert(m_data.size() > 0);

        auto compInstance = lookup(e);
        assert(compInstance.valid());

        if (m_destructionCallback)
            m_destructionCallback(getData(compInstance));

        if (m_data.size() == 1)
        {
            m_data.clear();
            m_lookup.clear();
            m_holders.clear();
        }
        else
        {
            auto lastEnt = m_holders.find(m_data.size() - 1)->second;
            std::swap(m_data[compInstance.id], m_data.back());
            m_lookup.find(lastEnt.id)->second = compInstance;
            m_holders.find(compInstance.id)->second = lastEnt;

            m_holders.erase(m_data.size() - 1);
            m_data.pop_back();
            m_lookup.erase(e.id);
        }
    }

    bool contains(Entity e)
    {
        assert(e.valid());
        return lookup(e).valid();
    }

    const T& getData(ComponentInstance inst) const
    {
        assert(inst.valid());

        return m_data[inst.id];
    }

    T& getData(ComponentInstance inst)
    {
        assert(inst.valid());

        return m_data[inst.id];
    }

    const T& getData(Entity e) const
    {
        return getData(lookup(e));
    }

    T& getData(Entity e)
    {
        return getData(lookup(e));
    }

    std::vector<T>& rawData() { return m_data; }

    size_t getSize() const
    {
        assert(m_data.size() == m_lookup.size());
        assert(m_lookup.size() == m_holders.size());

        return m_data.size();
    }

    void clear()
    {
        if (m_destructionCallback)
        {
            for (auto &data : m_data)
                m_destructionCallback(data);
        }

        m_data.clear();
        m_holders.clear();
        m_lookup.clear();
    }

    void cleanStep(const std::list<Entity> &deleted)
    {
        for (auto e : deleted)
        {
            if (contains(e))
                remove(e);
        }

        // TODO_ecs: shrinking to fit each n sec.
    }

    void setDestructionCallback(const DestructionCallback &callback) { m_destructionCallback = callback; }
};

}