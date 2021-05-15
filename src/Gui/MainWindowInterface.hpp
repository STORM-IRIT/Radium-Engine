#pragma once

#include <Gui/RaGui.hpp>

#include <QMainWindow>
#include <QMessageBox>
#include <memory>

namespace Ra {
namespace Engine {
namespace Rendering {
class Renderer;
}
} // namespace Engine

namespace Plugins {
class RadiumPluginInterface;
}

namespace Gui {
class Viewer;
class SelectionManager;
class Timeline;

/// Interface class for MainWindow
/// contains abstract methods that MainApplication uses.
class RA_GUI_API MainWindowInterface : public QMainWindow
{
    Q_OBJECT

  public:
    /// Constructor and destructor.
    explicit MainWindowInterface( QWidget* parent = nullptr ) : QMainWindow( parent ) {};
    virtual ~MainWindowInterface() {};

    /// Access the viewer, i.e. the rendering widget.
    virtual Ra::Gui::Viewer* getViewer() = 0;

    /// Access the selection manager.
    virtual Gui::SelectionManager* getSelectionManager() = 0;

    /// Access the timeline.
    virtual Gui::Timeline* getTimeline() = 0;

    /// Update the ui from the plugins loaded.
    virtual void updateUi( Plugins::RadiumPluginInterface* plugin ) = 0;

    /// Update the UI ( most importantly gizmos ) to the modifications of the
    /// engine
    virtual void onFrameComplete() = 0;

    /// Add render in the application: UI, viewer.
    virtual void addRenderer( const std::string& name,
                              std::shared_ptr<Engine::Rendering::Renderer> e ) = 0;

  public slots:
    /**
     * Called when a scene is ready to display to parameterize the application window and the
     * viewer.
     */
    virtual void prepareDisplay() = 0;

    /// Cleanup resources.
    virtual void cleanup() = 0;

    virtual void displayHelpDialog() {
        QMessageBox helpMsg;
        helpMsg.setText( "No help for this application yet !" );
        helpMsg.exec();
    }

  signals:
    /// Emitted when the closed button has been hit.
    void closed();

    /// Emitted when the viewer request OpenGL initialization of the engine
    void requestEngineOpenGLInitialization();
};

} // namespace Gui
} // namespace Ra
