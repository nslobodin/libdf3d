#pragma once

#include "Entity.h"
#include "WorldRenderingParams.h"

namespace df3d {

namespace game_impl { class EntityManager; class EntityLoader; }

class AudioComponentProcessor;
class StaticMeshComponentProcessor;
class ParticleSystemComponentProcessor;
class PhysicsComponentProcessor;
class TransformComponentProcessor;
class DebugNameComponentProcessor;
class Sprite2DComponentProcessor;
class RenderQueue;
class EntityComponentProcessor;
class Camera;
class TimeManager;

class DF3D_DLL World : utils::NonCopyable
{
    friend class EngineController;
    friend class RenderManager;

    unique_ptr<game_impl::EntityManager> m_entityManager;
    unique_ptr<game_impl::EntityLoader> m_entityLoader;

    unique_ptr<EntityComponentProcessor> m_audio;
    unique_ptr<EntityComponentProcessor> m_staticMeshes;
    unique_ptr<EntityComponentProcessor> m_vfx;
    unique_ptr<EntityComponentProcessor> m_physics;
    unique_ptr<EntityComponentProcessor> m_tranform;
    unique_ptr<EntityComponentProcessor> m_debugName;
    unique_ptr<EntityComponentProcessor> m_sprite2D;

    shared_ptr<Camera> m_camera;
    WorldRenderingParams m_renderingParams;

    bool m_paused = false;

    unique_ptr<TimeManager> m_timeMgr;

    std::list<Entity> m_recentlyRemovedEntities;

    void update();
    void collectRenderOperations(RenderQueue *ops);
    void cleanStep();

    World();
    void destroyWorld();

public:
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    Entity spawn(const Json::Value &entityResource);
    bool alive(Entity e);
    void destroy(Entity e);

    void pauseSimulation(bool paused);

    void setCamera(shared_ptr<Camera> camera) { m_camera = camera; }
    void setRenderingParams(const WorldRenderingParams &params) { m_renderingParams = params; }

    Camera& getCamera() { return *m_camera; }
    const Camera& getCamera() const { return *m_camera; }

    WorldRenderingParams& getRenderingParams() { return m_renderingParams; }
    const WorldRenderingParams& getRenderingParams() const { return m_renderingParams; }

    AudioComponentProcessor& audio();
    StaticMeshComponentProcessor& staticMesh();
    ParticleSystemComponentProcessor& vfx();
    PhysicsComponentProcessor& physics();
    TransformComponentProcessor& transform();
    DebugNameComponentProcessor& debugName();
    Sprite2DComponentProcessor& sprite2d();

    TimeManager& timeManager() { return *m_timeMgr; }
};

}
