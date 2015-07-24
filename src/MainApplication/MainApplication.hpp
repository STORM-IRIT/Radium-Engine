#include <memory>
#include <QApplication>
#include <QTime>
#include <MainApplication/Viewer/Viewer.hpp>

class QTimer;
namespace Ra { namespace Engine { class RadiumEngine;}}
namespace Ra { namespace Gui { class Viewer;}}
namespace Ra { namespace Gui { class MainWindow;}}

namespace Ra
{
    /// This class contains the main application logic. It owns the engine and the GUI.
    class MainApplication : public QApplication
    {
        Q_OBJECT

    public:
        MainApplication(int argc, char** argv);
        ~MainApplication();

    public slots:
        /// Advance the engine for one frame. Called by an internal timer.
        void radiumFrame();

        void loadFile(QString path);


        /// Callback slot for the viewer.
        void viewerReady( Gui::Viewer* viewer );

    private:
        /// Application main window and GUI root class.
        std::unique_ptr<Gui::MainWindow> m_mainWindow;

        /// Instance of the radium engine.
        std::unique_ptr<Engine::RadiumEngine> m_engine;

        /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Gui::Viewer* m_viewer;

        /// Timer to wake us up at every frame start.
        QTimer* m_frameTimer;

        /// Time since the last frame start.
        QTime m_frameTime;

    };

}