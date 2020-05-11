#pragma once

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined PluginB_EXPORTS
#    define PluginB_API DLL_EXPORT
#else
#    define PluginB_API DLL_IMPORT
#endif

#include <PluginBase/RadiumPluginInterface.hpp>

namespace PluginB_NS {

/**
 * This plugin call the example library,  compiled from another project when initialized.
 */
class PluginB_API PluginB : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_RADIUM_PLUGIN_METADATA
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    PluginB() = default;

    ~PluginB() override = default;

    void registerPlugin( const Ra::Plugins::Context& context ) override;

    bool doAddWidget( QString& name ) override { return false; }
    QWidget* getWidget() override { return nullptr; }
    bool doAddMenu() override { return false; }
    QMenu* getMenu() override { return nullptr; }
    bool doAddAction( int& nb ) override { return false; }
    QAction* getAction( int id ) override { return nullptr; }
};

} // namespace PluginB_NS
