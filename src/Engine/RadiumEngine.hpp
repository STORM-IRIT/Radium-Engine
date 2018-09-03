#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Singleton.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

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

/// The RadiumEngine class is the main scene storage.
class RA_ENGINE_API RadiumEngine {
    RA_SINGLETON_INTERFACE( RadiumEngine );

  public:
    RadiumEngine();

    ~RadiumEngine();

    /// Initialize all managers.
    void initialize();

    /// Clearup all managers and delete them.
    void cleanup();

    /// Output the list of Tasks to process at time \p dt.
    void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

    /// Register the given System with the given name.
    void registerSystem( const std::string& name, System* system );

    /// Return the System associated to the given name if it exists, nullptr otherwise.
    System* getSystem( const std::string& system ) const;

    /// Convenience function returning a Mesh from its entity and
    /// component names.
    /// When no RenderObject name is given, returns the mesh associated
    /// to the first render object.
    Mesh* getMesh( const std::string& entityName, const std::string& componentName,
                   const std::string& roName = std::string() ) const;

    /**
     * Try to loads the given file.
     * If no loader able to manage the fileformat of the file (determined based
     * on its extension), return false.
     * If a loader is found, creates the root entity of the loaded scene and
     * gives the content of the file to all systems to add components and
     * to this root entity.
     * @note Calling this method set the engine in the "loading state".
     * @param file
     * @return true if file is loaded, false else.
     */
    bool loadFile( const std::string& file );

    /**
     * Access to the content of the loaded file.
     * Acces to the content is only available at loading time. As soon as the
     * loaded file is released, its content is no more available outside the
     * Entity/Component architecture.
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

    /// Return the RenderObjectManager.
    RenderObjectManager* getRenderObjectManager() const;

    /// Return the EntityManager.
    EntityManager* getEntityManager() const;

    /// Return the SignalManager.
    SignalManager* getSignalManager() const;

    /// Register the given FileLoader.
    void registerFileLoader( std::shared_ptr<Asset::FileLoaderInterface> fileLoader );

    /// Return the list of FileLoaders.
    const std::vector<std::shared_ptr<Asset::FileLoaderInterface>>& getFileLoaders() const;

  private:
    /// The Engine Systems.
    std::map<std::string, std::shared_ptr<System>> m_systems;

    /// The Engine FileLoaders.
    std::vector<std::shared_ptr<Asset::FileLoaderInterface>> m_fileLoaders;

    /// The Engine RenderObjectManager.
    std::unique_ptr<RenderObjectManager> m_renderObjectManager;

    /// The Engine EntitytManager.
    std::unique_ptr<EntityManager> m_entityManager;

    /// The Engine SignalManager.
    std::unique_ptr<SignalManager> m_signalManager;

    /// The last loaded file data.
    std::unique_ptr<Asset::FileData> m_loadedFile;

    /// Whether a file has been successfully loaded.
    bool m_loadingState = false;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
