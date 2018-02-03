#ifndef RADIUMENGINE_BASEAPPLICATION_HPP_
#define RADIUMENGINE_BASEAPPLICATION_HPP_
#include <chrono>
#include <memory>
#include <vector>

#include <QApplication>

#include <Core/Time/Timer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

class QTimer;
namespace Ra
{
    namespace Core
    {
        class TaskQueue;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
        struct ItemEntry;
    }
}

namespace Ra
{
    namespace Gui
    {
        class Viewer;
    }

    namespace GuiBase{
        class MainWindowInterface;
    }
}

namespace Ra
{
    namespace Plugins
    {
        class RadiumPluginInterface;
    }
}

namespace Ra
{
namespace GuiBase{
    /// This class contains the main application logic. It owns the engine and the GUI.
    class BaseApplication : public QApplication
    {
        Q_OBJECT

    public:

        class WindowFactory{
        public:
            WindowFactory(){};
            virtual  Ra::GuiBase::MainWindowInterface *createMainWindow() const =0;
        };

        /// Setup the application, create main window and main connections.
        ///\param argc from main()
        ///\param argv from main()
        ///\param factory : a functor that instanciate the mainWindow
        BaseApplication( int argc, char** argv, const WindowFactory &factory, QString applicationName = "RadiumEngine", QString organizationName = "STORM-IRIT");
        ~BaseApplication();

        /// Advance the engine for one frame.
        void radiumFrame();

        bool isRunning() const { return !m_isAboutToQuit; }

        const Engine::RadiumEngine* getEngine () const { return m_engine.get();}

        uint getFrameCount() const { return m_frameCounter; }
    signals:
        /// Fired when the engine has just started, before the frame timer is set.
        void starting();

        /// Fired when the engine is about to stop.
        void stopping();

        /// Fired when the scene has changed.
        void sceneChanged( const Core::Aabb& );

        void updateFrameStats( const std::vector<FrameTimerData>& );

        void loadComplete();

        void selectedItem(const Ra::Engine::ItemEntry& entry);

    public slots:

        void loadFile( QString path );
        void framesCountForStatsChanged( uint count );
        void appNeedsToQuit();
        void initializeOpenGlPlugins();

        void setRealFrameRate( bool on );
        void setRecordFrames( bool on );
        void setRecordTimings( bool on );
        void setRecordGraph( bool on );

        void recordFrame();

        void onSelectedItem(const Ra::Engine::ItemEntry& entry) { emit selectedItem(entry); }

    protected:
        /// Create signal / slots connections
        void createConnections();

        /// Load plugins from the specified folder.
        /// If loadList is empty, attempts to load all DLLs in the folder (except those on the ignore list)
        /// If loadList contains names it will only look for DLLs in that folder with the given name.
        bool loadPlugins( const std::string& pluginsPath, const QStringList& loadList, const QStringList& ignoreList );

        void setupScene();
        void addBasicShaders();


        // Public variables, accessible through the mainApp singleton.
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
        Core::Timer::TimePoint m_lastFrameStart;

        uint m_frameCounter;
        uint m_frameCountBeforeUpdate;
        uint m_numFrames;
        uint m_maxThreads;
        std::vector<FrameTimerData> m_timerData;

        /// If true, use the wall clock to advance the engine. If false, use a fixed time step.
        bool m_realFrameRate;

        // Options to control monitoring and outputs
        /// Name of the folder where exported data goes
        std::string m_exportFoldername;

        /// If true, dump each frame to a PNG file.
        bool m_recordFrames;
        /// If true, print the detailed timings of each frame
        bool m_recordTimings;
        /// If true, print the task graph;
        bool m_recordGraph;

        bool m_isAboutToQuit;
    };
}
}
#endif // RADIUMENGINE_BASEAPPLICATION_HPP_
