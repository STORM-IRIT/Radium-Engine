#ifndef DUMMYPLUGIN_DUMMYPLUGIN_HPP
#define DUMMYPLUGIN_DUMMYPLUGIN_HPP

#include <DummyPluginMacros.hpp>

#include <QObject>
#include <QtPlugin>

#include <PluginBase/RadiumPluginInterface.hpp>

class QLabel;

namespace Ra {
namespace Engine {
class RadiumEngine;
}
} // namespace Ra

namespace DummyPlugin {
class DummySystem;

class DummyPlugin : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "foo.bar.RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    DummyPlugin() = default;
    ~DummyPlugin();

    void registerPlugin( const Ra::Plugins::Context& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

  private slots:
    void sayHello();

  private:
    DummySystem* m_system{nullptr};
    QLabel* m_label{nullptr};
};

} // namespace DummyPlugin

#endif // FANCYMESHPLUGIN_HPP_
