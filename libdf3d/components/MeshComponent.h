#pragma once

#include "NodeComponent.h"
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>

FWD_MODULE_CLASS(render, MeshData)
FWD_MODULE_CLASS(render, Material)
FWD_MODULE_CLASS(scene, DebugDrawNode)

namespace df3d { namespace components {

class DF3D_DLL MeshComponent : public NodeComponent
{
    void onEvent(components::Event ev);
    void onDraw(render::RenderQueue *ops);

    shared_ptr<render::MeshData> m_geometry;

    // AABB in model space.
    scene::AABB m_aabb;
    // Transformed AABB.
    scene::AABB m_transformedAABB;
    bool m_aabbDirty = true;
    bool m_transformedAabbDirty = true;
    void constructAABB();
    void constructTransformedAABB();

    // Bounding sphere.
    scene::BoundingSphere m_sphere;
    bool m_boundingSphereDirty = true;
    void constructBoundingSphere();

    // Oriented bb.
    scene::OBB m_obb;
    bool m_obbDirty = true;
    bool m_obbTransformationDirty = true;
    void constructOBB();

    MeshComponent();

public:
    MeshComponent(const char *meshFilePath);
    ~MeshComponent();

    void setGeometry(shared_ptr<render::MeshData> geometry);
    shared_ptr<render::MeshData> getGeometry() { return m_geometry; }
    bool isGeometryValid() const;

    void setMaterial(shared_ptr<render::Material> material, size_t submeshIdx);
    shared_ptr<render::Material> getMaterial(size_t submeshIdx);

    scene::AABB getAABB();
    scene::BoundingSphere getBoundingSphere();
    scene::OBB getOBB();

    shared_ptr<NodeComponent> clone() const;
};

} }