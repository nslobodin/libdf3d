#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace scene {

class Scene;
class Node;
class Camera;
class SceneManagerListener;

class DF3D_DLL SceneManager : boost::noncopyable
{
    friend class base::Controller;

    SceneManager();
    ~SceneManager();

    shared_ptr<Scene> m_currentScene;
    std::string m_currentSceneName;
    shared_ptr<Camera> m_camera;

    bool m_paused = false;

    std::list<shared_ptr<Node>> m_nodesMarkedForRemoval;
    std::list<SceneManagerListener *> m_listeners;

    // These are only for controller.
    bool init();
    void shutdown();

    void update(float dt);
    void cleanStep();

public:
    void clearCurrentScene();
    void pauseSimulation(bool pause);

    shared_ptr<Scene> setCurrentScene(const char *filePath);
    shared_ptr<Scene> getCurrentScene() const;

    void setCamera(shared_ptr<Camera> camera);
    shared_ptr<Camera> getCamera() const;

    // NOTE:
    // Not immediate removal.
    void removeNodeFromScene(shared_ptr<Node> node);
    void removeNodeFromScene(const char *name);

    void addNodeToScene(shared_ptr<Node> node);

    void registerListener(SceneManagerListener *listener);
    void unregisterListener(SceneManagerListener *listener);
};

} }