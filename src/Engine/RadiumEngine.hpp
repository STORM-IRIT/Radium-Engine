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
namespace Asset {
class FileLoaderInterface;
class FileData;
} // namespace Asset
} // namespace Core

namespace Engine {
class System;
class Entity;
class Component;
class Displayable;
class RenderObjectManager;
class EntityManager;
class SignalManager;
} // namespace Engine

namespace Engine {
/**
 * Engine main class : Manage all the systems and managers that are used by the engine module.
 * @see Documentation on Engine Object Model
 */
class RA_ENGINE_API RadiumEngine {
    RA_SINGLETON_INTERFACE( RadiumEngine );

  public:
    RadiumEngine();
    ~RadiumEngine();

    /**
     * Instantiate all object managers that make Radium Object Model functionnal.
     * Initialize internal resources and default states.
     */
    void initialize();

    /**
     * Free all resources acquired during initialize
     */
    void cleanup();

    /**
     * Builds the set of task that must be executed for the current frame.
     *
     * @see Documentation on Engine Object Model the what are tasks and what they can do
     * @param taskQueue the task queue that will be executed for the current frame
     * @param dt
     */
    void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

    /**
     * @param priority Value used to rank the systems (see more in description)
     *
     * System with high priority will always be used first. Systems with the same
     * priority are ranked randomly.
     * Default priority is 1 for all systems;
     *
     * @param name
     * @param system
     * @param priority
     */
    bool registerSystem( const std::string& name, System* system, int priority = 1 );

    /**
     * Get the named system
     * @param system
     * @return
     */
    System* getSystem( const std::string& system ) const;

    /** Convenience function returning a Mesh from its entity and
     * component names.
     * When no RenderObject name is given, returns the mesh associated
     * to the first render object.
     * @note : mark as deprecated as it must be either removed or reimplemented
     * @deprecated Will be removed from this class in the next release. A Mesh manager, that could
     * serve mesh by name will be implemented.
     */
    [[deprecated]] Displayable* getMesh( const std::string& entityName,
                                         const std::string& componentName,
                                         const std::string& roName = std::string() ) const;

    /**
     * Try to loads the given file.
     * If no loader is able to process the input fileformat (determined on the file extension),
     * return false. If a loader is found, creates the root entity of the loaded scene and gives the
     * content of the file to all systems to add components and to this root entity.
     * @note Calling this method set the engine in the "loading state".
     * @param file
     * @return true if file is loaded, false else.
     */
    bool loadFile( const std::string& file );

    /**
     * Access to the content of the loaded file.
     * Access to the content is only available at loading time. As soon as the loaded file is
     * released, its content is no more available outside the Entity/Component architecture.
     * @pre The Engine must be in "loading state".
     * @return
     */
    const Core::Asset::FileData& getFileData() const;

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
    /**
     * Get the RenderObject manager attached to the engine.
     * @note, the engine keep ownership on the pointer returned
     * @return the object manager
     */
    RenderObjectManager* getRenderObjectManager() const;
    /**
     * Get the entity manager attached to the engine.
     * @note, the engine keep ownership on the pointer returned
     * @return the entity manager
     */
    EntityManager* getEntityManager() const;

    /**
     * Get the signal manager attached to the engine.
     * @note, the engine keep ownership on the pointer returned
     * @return the signal manager
     */
    SignalManager* getSignalManager() const;

    /**
     * Register a new file loader to the engine.
     * @param fileLoader
     */
    void registerFileLoader( std::shared_ptr<Core::Asset::FileLoaderInterface> fileLoader );

    /**
     * Get the active file loaders from the engine
     * @return
     */
    const std::vector<std::shared_ptr<Core::Asset::FileLoaderInterface>>& getFileLoaders() const;

  private:
    using priority = int;
    using SystemKey = std::pair<priority, std::string>;
    // use transparent functors :
    // https://clang.llvm.org/extra/clang-tidy/checks/modernize-use-transparent-functors.html
    using SystemContainer = std::map<SystemKey, std::shared_ptr<System>, std::greater<>>;

    SystemContainer::const_iterator findSystem( const std::string& name ) const;
    SystemContainer::iterator findSystem( const std::string& name );

    /**
     * Stores the systems by priority.
     * \note For convenience, higher priority means that a system will be evaluated first.
     */
    SystemContainer m_systems;

    std::vector<std::shared_ptr<Core::Asset::FileLoaderInterface>> m_fileLoaders;

    std::unique_ptr<RenderObjectManager> m_renderObjectManager;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<SignalManager> m_signalManager;
    std::unique_ptr<Core::Asset::FileData> m_loadedFile;

    bool m_loadingState{false};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
