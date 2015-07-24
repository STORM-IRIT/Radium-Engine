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
    class MainApplication : public QApplication
    {
        Q_OBJECT

    public:
        MainApplication(int argc, char** argv);
        ~MainApplication();

    public slots:
        void viewerReady( Gui::Viewer* viewer );
        void radiumFrame();

    private:
        std::unique_ptr<Gui::MainWindow> m_mainWindow;
        std::unique_ptr<Engine::RadiumEngine> m_engine;

        QTimer* m_frameTimer;
        QTime m_frameTime;

        // Viewer belongs to MainWindow.
        Gui::Viewer* m_viewer;
    };

}