#ifndef RADIUMENGINE_PLUGININTERFACE_HPP
#define RADIUMENGINE_PLUGININTERFACE_HPP

#include <QObject>
#include <QtPlugin>

#include <Engine/Plugins/Interface.hpp>

namespace Ra {
namespace Engine {
namespace Plugin {

class PluginInterface : public QObject, public Interface {

    /// MACRO
    Q_OBJECT
    Q_PLUGIN_METADATA(IID INTERFACE_IID )
    Q_INTERFACES( Interface )

public:
    /// CONSTRUCTOR
    PluginInterface( QObject* parent = nullptr ) : QObject( parent ), Interface() { }

    /// DESTRUCTOR
    virtual ~PluginInterface() { }

    /// INTERFACE
    virtual std::string getName() const = 0;

public slots:
    virtual void run() = 0;

};

} // namespace Plugin
} // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_PLUGININTERFACE_HPP
