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

    /**
     * Try to loads the given file.
     * If no loader able to manage the fileformat of the file (determined based on its extension), return false.
     * If a loader is found, creates the root entity of the loaded scene and gives the content of the file to all
     * systems to add components and
     * to this root entity.
     * @note Calling this method set the engine in the "loading state".
     * @param file
     * @return true if file is loaded, false else.
     */
    bool loadFile( const std::string& file );

    /**
     * Access to the content of the loaded file.
     * Acces to the content is only available at loading time. As soon as the loaded file is released, its content is
     * no more available outside the Entity/Component architecture.
     * @pre The Engine must be in "loading state".
     * @return
     */
    const Asset::FileData& getFileData() const;

    /**
     * Release the content of the loaded file.
     * After calling this, the getFileData method is
      * @note Calling this method set the engine out of the "loading state".
    */
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

    bool m_loadingState = false;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
