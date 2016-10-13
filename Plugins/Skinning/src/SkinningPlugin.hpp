#ifndef SKINNINGLUGIN_HPP_
#define SKINNINGLUGIN_HPP_

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  Skinning_EXPORTS
    #define SKIN_PLUGIN_API DLL_EXPORT
#else
    #define SKIN_PLUGIN_API DLL_IMPORT
#endif

#include <QObject>
#include <QtPlugin>
#include <QFrame>
#include <QComboBox>
#include <PluginBase/RadiumPluginInterface.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
        struct ItemEntry;
    }
    namespace Guibase
    {
        class SelectionManager;
    }
}

namespace SkinningPlugin
{

class SkinningComponent;
class SkinningSystem;

class SkinningWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SkinningWidget( QWidget* parent = nullptr );
public slots:
    void setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp );

private slots:
    void onSkinningChanged( int  newType );

private:
        SkinningComponent* m_current;
        QComboBox* m_skinningSelect;
};

// Du to an ambiguous name while compiling with Clang, must differentiate plugin claas from plugin namespace
class SkinningPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

public:
    virtual ~SkinningPluginC();

    virtual void registerPlugin( const Ra::PluginContext& context ) override;

    virtual bool doAddWidget( QString& name ) override;
    virtual QWidget* getWidget() override;

    virtual bool doAddMenu() override;
    virtual QMenu* getMenu() override;


private slots:
    void onCurrentChanged( const QModelIndex& current , const QModelIndex& prev);

private:
    SkinningSystem* m_system;
    Ra::GuiBase::SelectionManager* m_selectionManager;
    SkinningWidget* m_widget;

};

} // namespace

#endif // SKINNINGPLUGIN_HPP_
