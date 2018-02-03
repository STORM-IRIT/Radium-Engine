#ifndef RADIUMENGINE_MAINWINDOWINTERFACE_HPP_
#define RADIUMENGINE_MAINWINDOWINTERFACE_HPP_

#include <GuiBase/RaGuiBase.hpp>

#include <QMainWindow>
#include <memory>

namespace Ra
{
namespace Gui
{
class Viewer;
}

namespace GuiBase
{
class SelectionManager;
}

namespace Plugins
{
class RadiumPluginInterface;
}

namespace Engine{
class Renderer;
}

}


namespace Ra {
namespace GuiBase {

/// Interface class for MainWindow
/// contains abstract methods that MainApplication uses.
class MainWindowInterface : public QMainWindow {
  Q_OBJECT

public:
  /// Constructor and destructor.
    explicit MainWindowInterface(QWidget *parent = nullptr){};
    virtual ~MainWindowInterface(){};

  /// Access the viewer, i.e. the rendering widget.
  virtual Ra::Gui::Viewer *getViewer() = 0;

  /// Access the selection manager.
  virtual GuiBase::SelectionManager *getSelectionManager() = 0;

  /// Update the ui from the plugins loaded.
  virtual void updateUi(Plugins::RadiumPluginInterface *plugin) = 0;

  /// Update the UI ( most importantly gizmos ) to the modifications of the
  /// engine/
  virtual void onFrameComplete() = 0;

  /// Add render in the application: UI, viewer.
  virtual void addRenderer(std::string name,
                           std::shared_ptr<Engine::Renderer> e) = 0;

public slots:
  /// Call after loading a new file to let the window resetview for instance.
  virtual void postLoadFile() = 0;
  /// Cleanup resources.
  virtual void cleanup() =0;

signals:
  /// Emitted when the closed button has been hit.
  void closed();
};
}
}

#endif // RADIUMENGINE_MAINWINDOWINTERFACE_HPP_
