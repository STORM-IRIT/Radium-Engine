#ifndef SKINNINGLUGIN_HPP_
#define SKINNINGLUGIN_HPP_

#include <SkinningPluginMacros.hpp>

#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QObject>
#include <QtPlugin>

namespace Ra {
namespace Engine {
class RadiumEngine;
struct ItemEntry;
} // namespace Engine
namespace Guibase {
class SelectionManager;
}
} // namespace Ra

namespace SkinningPlugin {

class SkinningComponent;
class SkinningSystem;

/// The SkinningWidget is the widget for the SkinningPlugin.
class SkinningWidget : public QFrame {
    Q_OBJECT

    friend class SkinningPluginC;

  public:
    explicit SkinningWidget( QWidget* parent = nullptr );

  public slots:
    /// Updates the UI according to the selected Component.
    void setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp );

  private slots:
    /// Sets the Skinning Method for the current Component.
    void onSkinningChanged( int newType );

    /// Sets LBS as the Skinning Method.
    void onLSBActionTriggered();

    /// Sets DQS as the Skinning Method.
    void onDQActionTriggered();

    /// Sets CoR as the Skinning Method.
    void onCoRActionTriggered();

  private:
    /// The current SkinningComponent.
    SkinningComponent* m_current;

    /// The comboBox for the Skinning Method.
    QComboBox* m_skinningSelect;

    /// The action for choosing LBS.
    QAction* m_actionLBS;

    /// The action for choosing DQS.
    QAction* m_actionDQ;

    /// The action for choosing CoR.
    QAction* m_actionCoR;
};

// Du to an ambiguous name while compiling with Clang, must differentiate plugin claas from plugin
// namespace
class SkinningPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    SkinningPluginC();

    virtual ~SkinningPluginC();

    void registerPlugin( const Ra::PluginContext& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  private slots:
    /// Register the selected Component as the current one.
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

  private:
    /// The SkinningSystem.
    SkinningSystem* m_system;

    /// The Skinning Widget.
    SkinningWidget* m_widget;

    /// The SelectionManager of the Engine.
    Ra::GuiBase::SelectionManager* m_selectionManager;
};

} // namespace SkinningPlugin

#endif // SKINNINGPLUGIN_HPP_
