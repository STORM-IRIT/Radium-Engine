#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <GuiBase/MainWindowInterface.hpp>
#include <GuiBase/RaGuiBase.hpp>
#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>
#include <GuiBase/TreeModel/EntityTreeModel.hpp>

#include "ui_MainWindow.h"
#include <QMainWindow>

#include <QEvent>
#include <qdebug.h>

namespace Ra {
namespace Engine {
class Entity;
}
} // namespace Ra

namespace Ra {
namespace Gui {
class EntityTreeModel;
class Viewer;
class MaterialEditor;
} // namespace Gui
} // namespace Ra

namespace Ra {
namespace Plugins {
class RadiumPluginInterface;
}
} // namespace Ra

namespace Ra {
namespace Gui {

/// This class manages most of the GUI of the application :
/// top menu, side toolbar and side dock.
class MainWindow : public Ra::GuiBase::MainWindowInterface, private Ui::MainWindow
{
    Q_OBJECT

  public:
    /// Constructor and destructor.
    explicit MainWindow( QWidget* parent = nullptr );
    virtual ~MainWindow();

    /// Access the viewer, i.e. the rendering widget.
    Viewer* getViewer() override;

    /// Access the selection manager.
    GuiBase::SelectionManager* getSelectionManager() override;

    /// Update the ui from the plugins loaded.
    void updateUi( Plugins::RadiumPluginInterface* plugin ) override;

    /// Update the UI ( most importantly gizmos ) to the modifications of the engine/
    void onFrameComplete() override;

    // Add render in the application: UI, viewer.
    void addRenderer(const std::string &name, std::shared_ptr<Engine::Renderer> e) override;

  public slots:
    /// Callback to rebuild the item model when the engine objects change.
    void onItemAdded( const Engine::ItemEntry& ent );

    void onItemRemoved( const Engine::ItemEntry& ent );

    // Frame timers ui slots
    void onUpdateFramestats( const std::vector<FrameTimerData>& stats );

    // Selection tools
    void onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

    // Gizmo buttons slots
    void gizmoShowNone();
    void gizmoShowTranslate();
    void gizmoShowRotate();
    void gizmoShowScale();

    // Keymapping configuration tool
    void reloadConfiguration();
    void loadConfiguration();

    /// Slot for the tree view checkboxes
    void setROVisible( Core::Utils::Index roIndex, bool visible );

    /// Reset the camera to see all visible objects
    void fitCamera();
    /// clear selection, fit camera and update material name in ui
    void postLoadFile(const std::string &filename) override;

    /// Slot for the "edit" button.
    void editRO();

    /// Cleanup resources.
    void cleanup() override;

    /// Show or hide all render objects
    void showHideAllRO();

  signals:
    /// Emitted when the frame loads
    void fileLoading( const QString path );

    /// Emitted when the user changes the timer box ("Frame average over count")
    void framescountForStatsChanged( int count );

    /// Emitted when a new item is selected. An invalid entry is sent when no item is selected.
    void selectedItem( const Engine::ItemEntry& entry );

  private:
    /// Connect qt signals and slots. Called once by the constructor.
    void createConnections();

    // Updates the Tracking info according to the selected feature, if any.
    void updateTrackedFeatureInfo();

    virtual void closeEvent( QCloseEvent* event ) override;

    /// Update displayed texture according to the current renderer
    void updateDisplayedTexture();

    /// Set the background color (updates viewer). If c is invalid, the color is fetch from
    /// QSettings.
    void updateBackgroundColor( QColor c = QColor() );

  private slots:
    /// Slot for the "load file" menu.
    void loadFile();

    /// Slot for the "material editor"
    void openMaterialEditor();

    /// Slot for the user changing the current shader
    void changeRenderObjectShader( const QString& shaderName );

    /// Slot for the user changing the current renderer
    void onCurrentRenderChangedInUI();

    /// Slot for the picking results from the viewer.
    void handlePicking( const Ra::Engine::Renderer::PickingResult& pickingResult );

    // Slot to init renderers once gl is ready
    void onGLInitialized();

    /// Slot to accept a new renderer
    void onRendererReady();

    /// Exports the mesh of the currently selected object to a file.
    void exportCurrentMesh();

    /// Remove the currently selected item (entity, component or ro)
    void deleteCurrentItem();

    /// Clears all entities and resets the camera.
    void resetScene();

    void toggleCirclePicking( bool on );

    void on_m_currentColorButton_clicked();

  private:
    /// Stores the internal model of engine objects for selection and visibility.
    GuiBase::ItemModel* m_itemModel;

    /// Stores and manages the current selection.
    GuiBase::SelectionManager* m_selectionManager;

    /// Widget to allow material edition.
    MaterialEditor* m_materialEditor;

    /// viewer widget
    Ra::Gui::Viewer* m_viewer;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
