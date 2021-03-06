#include "AABB.h"

#include <df3d/game/WorldSize.h>

namespace df3d {

AABB::AABB()
{
    reset();
}

AABB::~AABB()
{
}

void AABB::reset()
{
    m_min.x = m_min.y = m_min.z = MAX_COORD_VALUE;
    m_max.x = m_max.y = m_max.z = -MAX_COORD_VALUE;
}

void AABB::updateBounds(const glm::vec3 &point)
{
    if (point.x < m_min.x)
        m_min.x = point.x;

    if (point.x > m_max.x)
        m_max.x = point.x;

    if (point.y < m_min.y)
        m_min.y = point.y;

    if (point.y > m_max.y)
        m_max.y = point.y;

    if (point.z < m_min.z)
        m_min.z = point.z;

    if (point.z > m_max.z)
        m_max.z = point.z;
}

const glm::vec3 &AABB::minPoint() const
{
    return m_min;
}

const glm::vec3 &AABB::maxPoint() const
{
    return m_max;
}

bool AABB::isValid() const
{
    return m_min.x < m_max.x && m_min.y < m_max.y && m_min.z < m_max.z;
}

bool AABB::contains(const glm::vec3 &point) const
{
    // TODO:
    DF3D_ASSERT_MESS(false, "not implemented");
    return true;
}

bool AABB::intersects(const AABB &other) const
{
    if (!isValid() || !other.isValid())
        return false;

    return ((m_min.x >= other.m_min.x && m_min.x <= other.m_max.x) || (other.m_min.x >= m_min.x && other.m_min.x <= m_max.x)) &&
        ((m_min.y >= other.m_min.y && m_min.y <= other.m_max.y) || (other.m_min.y >= m_min.y && other.m_min.y <= m_max.y)) &&
        ((m_min.z >= other.m_min.z && m_min.z <= other.m_max.z) || (other.m_min.z >= m_min.z && other.m_min.z <= m_max.z));
}

bool AABB::intersects(const BoundingSphere &sphere) const
{
    // TODO:
    DF3D_ASSERT_MESS(false, "not implemented");
    return false;
}

glm::vec3 AABB::getCenter() const
{
    return (m_min + m_max) / 2.f;
}

void AABB::getCorners(glm::vec4 output[8]) const
{
    output[0] = glm::vec4(m_min.x, m_max.y, m_max.z, 1.0f);
    output[1] = glm::vec4(m_min.x, m_min.y, m_max.z, 1.0f);
    output[2] = glm::vec4(m_max.x, m_min.y, m_max.z, 1.0f);
    output[3] = glm::vec4(m_max.x, m_max.y, m_max.z, 1.0f);
    output[4] = glm::vec4(m_max.x, m_max.y, m_min.z, 1.0f);
    output[5] = glm::vec4(m_max.x, m_min.y, m_min.z, 1.0f);
    output[6] = glm::vec4(m_min.x, m_min.y, m_min.z, 1.0f);
    output[7] = glm::vec4(m_min.x, m_max.y, m_min.z, 1.0f);
}

}
