#ifndef RADIUMENGINE_BASEAPPLICATION_HPP_
#define RADIUMENGINE_BASEAPPLICATION_HPP_
#include <chrono>
#include <memory>
#include <vector>

#include <QApplication>

#include <Core/Utils/Timer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

class QTimer;

namespace Ra {
namespace Core {
class TaskQueue;
} // namespace Core
} // namespace Ra

namespace Ra {
namespace Engine {
class RadiumEngine;
class GeometrySystem;
struct ItemEntry;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {
class Viewer;
} // namespace Gui
namespace GuiBase {
class MainWindowInterface;
} // namespace GuiBase
} // namespace Ra

namespace Ra {
namespace Plugins {
class RadiumPluginInterface;
} // namespace Plugins
} // namespace Ra

namespace Ra {
namespace GuiBase {
/**
 * This class contains the main application logic. It owns the Engine and the gui.
 */
class RA_GUIBASE_API BaseApplication : public QApplication {
    Q_OBJECT

  public:
    /**
     * Factory for the MainWindow.
     */
    class WindowFactory {
      public:
        WindowFactory() {}
        virtual ~WindowFactory() {}
        virtual Ra::GuiBase::MainWindowInterface* createMainWindow() const = 0;
    };

    /**
     * Setup the application, create the main window and main connections.
     * \param argc parameters count from main().
     * \param argv parameters from main().
     * \param factory a functor that instanciate the mainWindow.
     * \param applicationName the name of the application.
     * \param organizationName the name of the organization delivering the application.
     */
    BaseApplication( int argc, char** argv, const WindowFactory& factory,
                     QString applicationName = "RadiumEngine",
                     QString organizationName = "STORM-IRIT" );

    virtual ~BaseApplication();

    /**
     * Advance the engine for one frame.
     */
    void radiumFrame();

    /**
     * Return false when the application is about to be closed, true otherwise.
     */
    bool isRunning() const { return !m_isAboutToQuit; }

    /**
     * Return the RadiumEngine of the application.
     */
    const Engine::RadiumEngine* getEngine() const { return m_engine.get(); }

    /**
     * Return the application's target framerate.
     */
    uint getFrameCount() const { return m_frameCounter; }

    const std::string& getExportFolderName() const { return m_exportFoldername; }

  signals:
    /**
     * Emitted when the engine has just started, before the frame timer is set.
     */
    void starting();

    /**
     * Emitted when the Engine is about to stop.
     */
    void stopping();

    /**
     * Emitted when the scene has changed.
     */
    void sceneChanged( const Core::Aabb& );

    /**
     * Emitted when an application frame has been processed.
     */
    void updateFrameStats( const std::vector<FrameTimerData>& );

    /**
     * Emitted when a file has been successfully loaded.
     */
    void loadComplete();

    /**
     * Emitted when an item has been selected.
     */
    void selectedItem( const Ra::Engine::ItemEntry& entry );

  public slots:
    /**
     * Load the given file.
     */
    bool loadFile( QString path );

    /**
     * Set the number of frames used to compute timings stats.
     */
    void framesCountForStatsChanged( uint count );

    /**
     * Tell the application to close itself after the current frame.
     */
    void appNeedsToQuit();

    /**
     * Initialize the OpenGL data for the application Plugins.
     */
    void initializeOpenGlPlugins();

    /**
     * Toggle on/off using the runtime or the target frame rate for running tasks and rendering.
     */
    void setRealFrameRate( bool on );

    /**
     * Toggle on/off recording the rendered frames.
     */
    void setRecordFrames( bool on );

    /**
     * Toggle on/off printing the timing stats to the standard output.
     */
    void setRecordTimings( bool on );

    /**
     * Toggle on/off printing the Task dependency graph on the standard output.
     */
    void setRecordGraph( bool on );

    /**
     * Saves the rendered frame to a file.
     */
    void recordFrame();

    /**
     * Fire selectedItem().
     */
    void onSelectedItem( const Ra::Engine::ItemEntry& entry ) { emit selectedItem( entry ); }

  protected:
    /**
     * Create signal / slots connections.
     */
    void createConnections();

    /**
     * Load plugins from the specified folder.
     * If loadList is empty, attempts to load all DLLs in the folder (except
     * those on the ignore list).
     * If loadList contains names it will only look for DLLs in that folder with
     * the given name.
     */
    bool loadPlugins( const std::string& pluginsPath, const QStringList& loadList,
                      const QStringList& ignoreList );

    /**
     * Initialize the scene with basic objects, i.e.\ world frame and grid floor.
     */
    void setupScene();

    /**
     * Initialize the basic rendering shaders.
     */
    void addBasicShaders();

  public:
    /// Application main window and GUI root class.
    std::unique_ptr<GuiBase::MainWindowInterface> m_mainWindow;

    /// Instance of the radium engine.
    std::unique_ptr<Engine::RadiumEngine> m_engine;

    /// Task queue for processing tasks.
    std::unique_ptr<Core::TaskQueue> m_taskQueue;

    /// Number of frames per second to generate.
    uint m_targetFPS;

  protected:
    /// Plugins that need to be initialized once OpenGL is ready
    std::vector<Ra::Plugins::RadiumPluginInterface*> m_openGLPlugins;

    /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
    Gui::Viewer* m_viewer;

    /// Timer to wake us up at every frame start.
    QTimer* m_frameTimer;

    /// Time since the last frame start.
    Core::Utils::TimePoint m_lastFrameStart;

    /// The total number of frames since the application started.
    uint m_frameCounter;

    /// The number of frames used to compute timings stats.
    uint m_frameCountBeforeUpdate;

    /**
     * The number of frames to be processed before closing the application.
     * \note A value of 0 means no frame limit.
     */
    uint m_numFrames;

    /// The maximal number of threads allowed for the application.
    uint m_maxThreads;

    /// The timings data for the lattest frame.
    std::vector<FrameTimerData> m_timerData;

    /// Whether to use the wall clock or a fixed time step to advance the engine.
    bool m_realFrameRate;

    /// Name of the folder where exported data goes
    std::string m_exportFoldername;

    /// Whether to record frames into image files.
    bool m_recordFrames;

    /// Whether to print timings stat to the standard output.
    bool m_recordTimings;

    /// Whether to print the Task dependency graph to the standard ouput.
    bool m_recordGraph;

    /// Whether the application is about to close.
    bool m_isAboutToQuit;
};
} // namespace GuiBase
} // namespace Ra
#endif // RADIUMENGINE_BASEAPPLICATION_HPP_
