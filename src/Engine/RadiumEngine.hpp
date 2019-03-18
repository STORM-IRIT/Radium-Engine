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
class Mesh;
class RenderObjectManager;
class EntityManager;
class SignalManager;
} // namespace Engine

namespace Engine {
/**
 * The Radium Engine main class: manages all the Systems and Managers that are
 * used by the Engine module.
 * \see Documentation on Engine Object Model
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
     * Free all resources acquired during initialize.
     */
    void cleanup();

    /**
     * Is called at the end of the frame to synchronize any data
     * that may have been updated during the frame's multithreaded processing.
     */
    void endFrameSync();

    /**
     * Builds the set of task that must be executed for the current frame.
     * \param taskQueue the task queue that will be executed for the current frame.
     * \param dt the time elapsed since the last frame.
     *
     * \see The Radium Engine programmer manual documentation for what are Tasks
     *      and what they can do.
     */
    void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

    /**
     * Register the given System with the given name and priority.
     * System with high priority will always be used first.
     * Systems with the same priority are ranked randomly.
     * Default priority is 1 for all Systems.
     */
    bool registerSystem( const std::string& name, System* system, int priority = 1 );

    /**
     * Return the System with the given name.
     */
    System* getSystem( const std::string& system ) const;

    /**
     * Convenience function returning a Mesh from its Entity and Component names.
     * When no RenderObject name is given, returns the mesh associated
     * to the first RenderObject.
     * \note Marked as deprecated as it must be either removed or reimplemented.
     * \deprecated Will be removed from this class in the next release.
     *             A MeshManager, that could store meshes by name will be implemented.
     */
    [[deprecated]] Mesh* getMesh( const std::string& entityName, const std::string& componentName,
                                  const std::string& roName = std::string() ) const;

    /// \name File loading
    /// \{

    /**
     * Try to loads the given file.
     *
     * If no loader is able to process the input fileformat (determined on the
     * file extension), then returns false.
     *
     * If a loader is found, creates the root entity of the loaded scene and gives the
     * content of the file to all systems to add components and to this root entity.
     *
     * \return true if file is loaded, false otherwise.
     * \note Calling this method puts the Engine in the "loading state".
     */
    bool loadFile( const std::string& file );

    /**
     * Access to the content of the loaded file.
     * Access to the content is only available at loading time.
     * As soon as the loaded file is released, its content is no more available
     * outside the Entity/Component architecture.
     * \pre The Engine must be in "loading state".
     */
    const Core::Asset::FileData& getFileData() const;

    /**
     * Release the content of the loaded file.
     * \note Calling this method set the engine out of the "loading state".
     */
    void releaseFile();

    /**
     * Register a new file loader to the Engine.
     */
    void registerFileLoader( std::shared_ptr<Core::Asset::FileLoaderInterface> fileLoader );

    /**
     * Return the active file loaders from the Engine.
     */
    const std::vector<std::shared_ptr<Core::Asset::FileLoaderInterface>>& getFileLoaders() const;
    /// \}

    /// \name Manager getters
    /// \{

    /**
     * Return the RenderObject manager attached to the Engine.
     * \note The Engine keep ownership on the pointer returned.
     */
    RenderObjectManager* getRenderObjectManager() const;

    /**
     * Return the EntityManager attached to the Engine.
     * \note The Engine keep ownership on the pointer returned.
     */
    EntityManager* getEntityManager() const;

    /**
     * Return the SignalManager attached to the Engine.
     * \note The Engine keep ownership on the pointer returned.
     */
    SignalManager* getSignalManager() const;
    /// \}

  private:
    using priority = int;
    using SystemKey = std::pair<priority, std::string>;
    // use transparent functors :
    // https://clang.llvm.org/extra/clang-tidy/checks/modernize-use-transparent-functors.html
    using SystemContainer = std::map<SystemKey, std::shared_ptr<System>, std::greater<>>;

    /**
     * Searches for the System with name \p name.
     * \return a const_iterator to the System if found, end() otherwise.
     */
    SystemContainer::const_iterator findSystem( const std::string& name ) const;

    /**
     * Searches for the System with name \p name.
     * \return an iterator to the System if found, end() otherwise.
     */
    SystemContainer::iterator findSystem( const std::string& name );

    /**
     * Stores the Systems by priority.
     * \note For convenience, higher priority means that a system will be evaluated first.
     */
    SystemContainer m_systems;

    /// The Engine FileLoaders.
    std::vector<std::shared_ptr<Core::Asset::FileLoaderInterface>> m_fileLoaders;

    /// The Engine RenderObjectManager.
    std::unique_ptr<RenderObjectManager> m_renderObjectManager;

    /// The Engine EntitytManager.
    std::unique_ptr<EntityManager> m_entityManager;

    /// The Engine SignalManager.
    std::unique_ptr<SignalManager> m_signalManager;

    /// The last loaded file data.
    std::unique_ptr<Core::Asset::FileData> m_loadedFile;

    /// Whether a file is being loaded.
    bool m_loadingState{false};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
