#pragma once
#include <Engine/RaEngine.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/Singleton.hpp>

#include <map>
#include <memory>
#include <stack>
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
class RA_ENGINE_API RadiumEngine
{
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
     * Register default shaders, materials and named strings
     */
    void registerDefaultPrograms();

    /**
     * Free all resources acquired during initialize
     */
    void cleanup();

    /**
     * Builds the set of task that must be executed for the current frame.
     *
     * @see Documentation on Engine Object Model the what are tasks and what they can do
     * @param taskQueue the task queue that will be executed for the current frame
     * @param dt        the time elapsed since the last frame in seconds.
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
     * After calling this, the getFileData method must not be called for this file
     * @param filename the name of the file to release. If empty, releases the last loaded file.
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

    /**
     * Compute the aabb of the scene by aggregating aabb from entity.
     */
    Core::Aabb computeSceneAabb() const;

    /**
     * Save currently bound OpenGL Framebuffer and viewport on the internal
     * Engine stack. You can restore the state afterward with popFboAndViewport()
     */
    void pushFboAndViewport();

    /**
     * Restore  OpenGL Framebuffer and viewport from the internal
     * Engine stack. The state have to be previously saved with pushFboAndViewport()
     * Error message if stack is empty.
     */
    void popFboAndViewport();

    /// \name Time Management.
    /// {

    /**
     * \brief Toggles Real-time (on) or Constant (off) time flow.
     *
     * In Real-time mode, time flows according to the actually elapsed time
     * between consecutive frames, while in constant mode time flows according
     * to a fixed time step.
     */
    void setRealTime( bool realTime );

    /**
     * \brief Indicates if Real-time time flow is on (Constant-time is off) .
     * \see isConstantTime
     */
    bool isRealTime() const;

    /**
     * \brief Indicates if Constant-time time flow is on (Real-time is off) .
     * \see isRealTime
     */
    bool isConstantTime() const;

    /**
     * \brief Sets the time delta between two frames for Constant-time time flow.
     * \param forceConstantTime If true, enforces Constant-time time mode by calling
     *                          setRealTime(false).
     *                          If false, doesn't change the time mode.
     * \return true if Constant-time flow is used.
     * \see isConstantTime
     */
    bool setConstantTimeStep( Scalar dt, bool forceConstantTime = false );

    /**
     * \brief Activates or disables ForwardBackward time flow.
     *
     * When ForwardBackward mode is disabled (default), time will flow through
     * the time window and restart over.
     * When ForwardBackward mode is enabled, time will flow back and forth
     * in the time window.
     * \note If `end` time is negative, the ForwardBackward mode doesn't apply
     *       and time just flows indefinitely.
     */
    void setForwardBackward( bool mode );

    /**
     * Activates or stops time flow.
     */
    void play( bool isPlaying );

    /**
     * Activates time flow for one frame.
     */
    void step();

    /**
     * Resets time to the `start` time of the time window.
     * \note Also stops time flow.
     */
    void resetTime();

    /**
     * Sets time to \p t.
     * \note \p t can be any time value, regardless of the time window or time
     *       flow modes. However as soon as time flows, \p t will be adapted.
     */
    void setTime( Scalar t );

    /**
     * Sets the `start` time for the time window.
     * \note A negative start time is considered as 0.
     * \note If the `start` time comes after the `end` time, then time will
     *       flow indefinitely.
     */
    void setStartTime( Scalar t );

    /**
     * Returns the `start` time for the time window.
     */
    Scalar getStartTime() const;

    /**
     * Sets the `end` time for the time window.
     * \note A negative end time makes the time flow indefinitely, whether
     *       in ForwardBackward mode or not.
     * \note If the `start` time comes after the `end` time, then time will
     *       flow indefinitely.
     */
    void setEndTime( Scalar t );

    /**
     * Returns the `end` time for the time window.
     */
    Scalar getEndTime() const;

    /**
     * \returns the current time.
     */
    Scalar getTime() const;

    /**
     * \returns the current frame.
     */
    uint getFrame() const;
    /// \}

  private:
    using Priority  = int;
    using SystemKey = std::pair<Priority, std::string>;
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

    bool m_loadingState {false};

    /// For internal resources management in a filesystem
    std::string m_resourcesRootDir;

    /// active fbo and viewport management, this class handle fbo and viewport
    /// values so that one can save and restore these value with pushFboAndViewport and
    /// popFboAndViewport
    class FboAndViewport
    {
      public:
        FboAndViewport( int fbo, std::array<int, 4>&& viewport ) :
            m_fbo {fbo}, m_viewport {viewport} {}
        int m_fbo;
        std::array<int, 4> m_viewport;
    };

    /// Stack of saved fbo and viewport values @see pushFboAndViewport popFboAndViewport
    std::stack<FboAndViewport> m_fboAndViewportStack;

    struct TimeData {
        /**
         * Update the current time from the current frame number, according to the
         * time flow policy (ForwardBackward or loop around).
         */
        void updateTime();

        Scalar m_dt;               ///< The time delta between 2 consecutive frames.
        Scalar m_startTime {0_ra}; ///< The `start` time for the time window.
        Scalar m_endTime;          ///< The `end` time for the time window.
        Scalar m_time {0_ra};      ///< The current time.
        bool m_play {false};       ///< Shall time flow.
        bool m_singleStep {true};  ///< Shall time flow for only one frame.
        bool m_realTime {false};   ///< Whether we use the effective time flow or the constant one.
        bool m_forwardBackward {false}; ///< Is PingPong mode enabled.
    };

    TimeData m_timeData;
};

} // namespace Engine
} // namespace Ra
