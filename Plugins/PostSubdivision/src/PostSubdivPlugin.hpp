#ifndef POSTSUBDIV_PLUGIN_HPP_
#define POSTSUBDIV_PLUGIN_HPP_

#include <PostSubdivPluginMacros.hpp>

#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QObject>
#include <QSpinBox>
#include <QtPlugin>

#include <UI/PostSubdivUI.h>

namespace Ra {
namespace Engine {
class RadiumEngine;
struct ItemEntry;
} // namespace Engine
namespace Guibase {
class SelectionManager;
}
} // namespace Ra

namespace PostSubdivPlugin {

class PostSubdivComponent;
class PostSubdivSystem;

// Du to an ambiguous name while compiling with Clang, must differentiate plugin claas from plugin
// namespace
class PostSubdivPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    virtual ~PostSubdivPluginC();

    virtual void registerPlugin( const Ra::PluginContext& context ) override;

    virtual bool doAddWidget( QString& name ) override;
    virtual QWidget* getWidget() override;

    virtual bool doAddMenu() override;
    virtual QMenu* getMenu() override;

    virtual bool doAddAction( int& nb ) override;
    virtual QAction* getAction( int id ) override;

  private slots:
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );
    void onSubdivMethodChanged( int method );
    void onSubdivIterChanged( int iter );

  private:
    void setCurrent( const Ra::Engine::ItemEntry& entry, PostSubdivComponent* comp );

  private:
    Ra::GuiBase::SelectionManager* m_selectionManager;
    PostSubdivUI* m_widget;
    PostSubdivSystem* m_system;
    PostSubdivComponent* m_current;
};

} // namespace PostSubdivPlugin

#endif // POSTSUBDIV_PLUGIN_HPP_
