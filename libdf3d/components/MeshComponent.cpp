#include "df3d_pch.h"
#include "MeshComponent.h"

#include <scene/Node.h>
#include <scene/Frustum.h>
#include <scene/Camera.h>
#include <components/TransformComponent.h>
#include <render/MeshData.h>
#include <render/VertexIndexBuffer.h>
#include <render/SubMesh.h>
#include <render/RenderQueue.h>
#include <render/Material.h>
#include <render/RenderPass.h>
#include <render/Technique.h>
#include <base/SystemsMacro.h>

namespace df3d { namespace components {

bool MeshComponent::isInFov()
{
    if (m_frustumCullingDisabled)
        return true;

    return g_sceneManager->getCamera()->getFrustum().sphereInFrustum(getBoundingSphere());
}

void MeshComponent::onEvent(components::ComponentEvent ev)
{
    if (ev == components::ComponentEvent::POSITION_CHANGED ||
        ev == components::ComponentEvent::ORIENTATION_CHANGED ||
        ev == components::ComponentEvent::SCALE_CHANGED)
    {
        m_transformedAabbDirty = true;
        m_obbTransformationDirty = true;
    }

    if (ev == components::ComponentEvent::SCALE_CHANGED)
    {
        m_boundingSphereDirty = true;
    }
}

void MeshComponent::onDraw(render::RenderQueue *ops)
{
    // If geometry data has not been loaded yet.
    if (!m_geometry || !m_geometry->valid())
        return;

    if (!m_visible || !isInFov())
        return;

    // Include all the submeshes.
    const auto &submeshes = m_geometry->getSubMeshes();
    for (auto &sm : submeshes)
    {
        auto tech = sm->getMaterial()->getCurrentTechnique();
        auto vb = sm->getVertexBuffer();
        auto ib = sm->getIndexBuffer();
        if (!tech || !vb)
            continue;

        size_t passCount = tech->getPassCount();
        for (size_t passidx = 0; passidx < passCount; passidx++)
        {
            render::RenderOperation newoperation;
            auto passProps = tech->getPass(passidx);

            newoperation.worldTransform = m_holder->transform()->getTransformation();
            newoperation.vertexData = vb;
            newoperation.indexData = ib;
            newoperation.passProps = passProps;

            if (passProps->isTransparent())
                ops->transparentOperations.push_back(newoperation);
            else
            {
                if (passProps->isLit())
                    ops->litOpaqueOperations.push_back(newoperation);
                else
                    ops->notLitOpaqueOperations.push_back(newoperation);
            }
        }
    }
}

void MeshComponent::constructAABB()
{
    m_aabb.constructFromGeometry(m_geometry);

    m_aabbDirty = !m_aabb.isValid();
}

void MeshComponent::constructTransformedAABB()
{
    m_transformedAABB.reset();

    if (!m_aabb.isValid())
        return;

    // Get the corners of original AABB (model-space).
    std::vector<glm::vec3> aabbCorners(8);
    m_aabb.getCorners(aabbCorners);

    // Create new AABB from the corners of the original also applying world transformation.
    auto tr = getHolder()->transform()->getTransformation();
    for (auto &p : aabbCorners)
    {
        auto trp = tr * glm::vec4(p, 1.0f);
        m_transformedAABB.updateBounds(glm::vec3(trp));
    }

    m_transformedAabbDirty = false;
}

void MeshComponent::constructBoundingSphere()
{
    m_sphere.constructFromGeometry(m_geometry);

    // Update transformation.
    auto pos = m_holder->transform()->getPosition();
    auto scale = m_holder->transform()->getScale();
    m_sphere.setPosition(pos);

    auto dir = m_sphere.getRadius() * glm::normalize(pos + glm::vec3(1.0, 1.0f, 1.f));
    dir = glm::mat3(glm::scale(scale)) * dir;

    m_sphere.setRadius(glm::length(dir));

    m_boundingSphereDirty = false;
}

void MeshComponent::updateBoundingSpherePosition()
{
    m_sphere.setPosition(m_holder->transform()->getPosition());
}

void MeshComponent::constructOBB()
{
    m_obb.constructFromGeometry(m_geometry);
    m_obb.setTransformation(m_holder->transform()->getTransformation());

    m_obbDirty = !m_obb.isValid();
    m_obbTransformationDirty = !m_obb.isValid();
}

MeshComponent::MeshComponent()
    : NodeComponent(MESH)
{

}

MeshComponent::MeshComponent(const char *meshFilePath)
    : MeshComponent()
{
    setGeometry(g_resourceManager->createMeshData(meshFilePath, ResourceLoadingMode::ASYNC));
}

MeshComponent::~MeshComponent()
{

}

void MeshComponent::setGeometry(shared_ptr<render::MeshData> geometry)
{
    if (!geometry)
    {
        base::glog << "Can not set null geometry to a mesh node" << base::logwarn;
        return;
    }

    m_geometry = geometry;
    m_aabbDirty = true;
    m_boundingSphereDirty = true;
    m_obbDirty = true;
    m_obbTransformationDirty = true;
    m_transformedAabbDirty = true;
}

bool MeshComponent::isGeometryValid() const
{
    return m_geometry->valid();
}

void MeshComponent::setMaterial(shared_ptr<render::Material> material, size_t submeshIdx)
{
    if (!m_geometry->valid())
    {
        base::glog << "Can not set material for non valid mesh" << base::logwarn;
        return;
    }

    if (submeshIdx >= m_geometry->getSubMeshes().size())
    {
        base::glog << "Invalid submesh index passed to MeshNode::setMaterial" << base::logwarn;
        return;
    }

    m_geometry->getSubMeshes()[submeshIdx]->setMaterial(material);
}

shared_ptr<render::Material> MeshComponent::getMaterial(size_t submeshIdx)
{
    if (!m_geometry->valid())
    {
        base::glog << "Can not get material from not valid mesh" << base::logwarn;
        return nullptr;
    }

    if (submeshIdx >= m_geometry->getSubMeshes().size())
    {
        base::glog << "Invalid submesh index passed to MeshNode::getMaterial" << base::logwarn;
        return nullptr;
    }

    return m_geometry->getSubMeshes()[submeshIdx]->getMaterial();
}

scene::AABB MeshComponent::getAABB()
{
    if (!m_geometry->valid())
        return scene::AABB();       // No valid AABB for non valid geometry.

    if (m_aabbDirty)
        constructAABB();

    if (m_transformedAabbDirty)
    {
        constructTransformedAABB();
    }

    return m_transformedAABB;
}

scene::BoundingSphere MeshComponent::getBoundingSphere()
{
    if (m_boundingSphereDirty)
    {
        constructBoundingSphere();
    }

    updateBoundingSpherePosition();

    return m_sphere;
}

scene::OBB MeshComponent::getOBB()
{
    if (m_obbDirty)
        constructOBB();

    if (m_obbTransformationDirty)
    {
        m_obbTransformationDirty = false;
    }

    return m_obb;
}

std::string MeshComponent::getMeshFilePath() const
{
    return m_geometry->getGUID();
}

shared_ptr<NodeComponent> MeshComponent::clone() const
{
    auto retRes = shared_ptr<MeshComponent>(new MeshComponent());

    // Clone mesh node fields.
    retRes->m_geometry = m_geometry->clone();
    retRes->m_aabb = m_aabb;
    retRes->m_transformedAABB = m_transformedAABB;
    retRes->m_aabbDirty = m_aabbDirty;
    retRes->m_transformedAabbDirty = m_transformedAabbDirty;
    retRes->m_boundingSphereDirty = m_boundingSphereDirty;
    retRes->m_sphere = m_sphere;
    retRes->m_obb = m_obb;
    retRes->m_obbDirty = m_obbDirty;
    retRes->m_obbTransformationDirty = m_obbTransformationDirty;
    retRes->m_visible = m_visible;
    retRes->m_frustumCullingDisabled = m_frustumCullingDisabled;

    return retRes;
}

} }
