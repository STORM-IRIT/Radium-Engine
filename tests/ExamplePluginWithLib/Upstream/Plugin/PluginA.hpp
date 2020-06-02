#pragma once

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined PluginA_EXPORTS
#    define PluginA_API DLL_EXPORT
#else
#    define PluginA_API DLL_IMPORT
#endif

#include <PluginBase/RadiumPluginInterface.hpp>

namespace PluginA_NS {

/**
 * This plugin just call the example library when initialized.
 */
class PluginA_API PluginA : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_RADIUM_PLUGIN_METADATA
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    PluginA() = default;

    ~PluginA() override = default;

    void registerPlugin( const Ra::Plugins::Context& context ) override;

    bool doAddWidget( QString& name ) override { return false; }
    QWidget* getWidget() override { return nullptr; }
    bool doAddMenu() override { return false; }
    QMenu* getMenu() override { return nullptr; }
    bool doAddAction( int& nb ) override { return false; }
    QAction* getAction( int id ) override { return nullptr; }
};

} // namespace PluginA_NS
