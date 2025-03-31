#pragma once

#include <Core/CoreMacros.hpp>
#include <PluginBase/RaPluginBase.hpp>
/// Defines the correct macro to export dll symbols.
#if defined DownstreamPlugin_EXPORTS
#    define DownstreamPlugin_API DLL_EXPORT
#else
#    define DownstreamPlugin_API DLL_IMPORT
#endif

#include <PluginBase/RadiumPluginInterface.hpp>

class QAction;
class QMenu;
class QString;
class QWidget;
namespace Ra {
namespace Plugins {
class Context;
} // namespace Plugins
} // namespace Ra

namespace PluginB_NS {

/**
 * This plugin call the example library,  compiled from another project when initialized.
 */
class DownstreamPlugin_API PluginB : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_RADIUM_PLUGIN_METADATA
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    PluginB() = default;

    ~PluginB() override = default;

    void registerPlugin( const Ra::Plugins::Context& context ) override;

    bool doAddWidget( QString& ) override { return false; }
    QWidget* getWidget() override { return nullptr; }
    bool doAddMenu() override { return false; }
    QMenu* getMenu() override { return nullptr; }
    bool doAddAction( int& ) override { return false; }
    QAction* getAction( int ) override { return nullptr; }
};

} // namespace PluginB_NS
