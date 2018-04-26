#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Singleton.hpp>

#include <map>
#include <vector>
#include <memory>
#include <string>

namespace Ra {
namespace Core {
class TaskQueue;
struct MouseEvent;
struct KeyEvent;
} // namespace Core

namespace Engine {
class System;
class Entity;
class Component;
class Mesh;
class RenderObjectManager;
class EntityManager;
class SignalManager;
} // namespace Engine

namespace Asset {
class FileLoaderInterface;
class FileData;
} // namespace Asset
} // namespace Ra

namespace Ra {
namespace Engine {
class RA_ENGINE_API RadiumEngine {
    RA_SINGLETON_INTERFACE( RadiumEngine );

  public:
    RadiumEngine();
    ~RadiumEngine();

    void initialize();
    void cleanup();

    void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

    void registerSystem( const std::string& name, System* system );
    System* getSystem( const std::string& system ) const;

    /// Convenience function returning a Mesh from its entity and
    /// component names.
    /// When no RenderObject name is given, returns the mesh associated
    /// to the first render object.
    Mesh* getMesh( const std::string& entityName, const std::string& componentName,
                   const std::string& roName = std::string() ) const;

    bool loadFile( const std::string& file );

    const Asset::FileData& getFileData() const;

    void releaseFile();

    /// Is called at the end of the frame to synchronize any data
    /// that may have been updated during the frame's multithreaded processing.
    void endFrameSync();

    /// Manager getters
    RenderObjectManager* getRenderObjectManager() const;
    EntityManager* getEntityManager() const;
    SignalManager* getSignalManager() const;

    void registerFileLoader( std::shared_ptr<Asset::FileLoaderInterface> fileLoader );

    const std::vector<std::shared_ptr<Asset::FileLoaderInterface>>& getFileLoaders() const;

  private:
    std::map<std::string, std::shared_ptr<System>> m_systems;

    std::vector<std::shared_ptr<Asset::FileLoaderInterface>> m_fileLoaders;

    std::unique_ptr<RenderObjectManager> m_renderObjectManager;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<SignalManager> m_signalManager;
    std::unique_ptr<Asset::FileData> m_loadedFile;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
