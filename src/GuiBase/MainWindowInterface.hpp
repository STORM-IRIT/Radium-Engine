#ifndef RADIUMENGINE_MAINWINDOWINTERFACE_HPP_
#define RADIUMENGINE_MAINWINDOWINTERFACE_HPP_

#include <GuiBase/RaGuiBase.hpp>

#include <QMainWindow>
#include <memory>

namespace Ra {
namespace Gui {
class Viewer;
}

namespace GuiBase {
class SelectionManager;
}

namespace Plugins {
class RadiumPluginInterface;
}

namespace Engine {
class Renderer;
}

} // namespace Ra

namespace Ra {
namespace GuiBase {

/**
 * Interface class for MainWindow.
 * Contains abstract methods that MainApplication uses.
 */
class RA_GUIBASE_API MainWindowInterface : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindowInterface( QWidget* parent = nullptr ) {}

    virtual ~MainWindowInterface() {}

    /**
     * Access the Viewer, i.e.\ the rendering widget.
     */
    virtual Ra::Gui::Viewer* getViewer() = 0;

    /**
     * Access the SelectionManager.
     */
    virtual GuiBase::SelectionManager* getSelectionManager() = 0;

    /**
     * Update the gui from the loaded plugins.
     */
    virtual void updateUi( Plugins::RadiumPluginInterface* plugin ) = 0;

    /**
     * Update the gui (most importantly gizmos) according to the modifications
     * of the Engine.
     */
    virtual void onFrameComplete() = 0;

    /**
     * Add a Renderer to the application.
     */
    virtual void addRenderer( std::string name, std::shared_ptr<Engine::Renderer> e ) = 0;

  public slots:
    /**
     * Notify a file has been corectly loaded.
     * Updates what must be updated w.r.t.\ the new scene configuration
     * (e.g.\ Camera, gui, ...).
     */
    virtual void postLoadFile() = 0;

    /**
     * Cleanup resources.
     */
    virtual void cleanup() = 0;

  signals:
    /**
     * Emitted when the closed button has been hit.
     */
    void closed();
};
} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOWINTERFACE_HPP_
