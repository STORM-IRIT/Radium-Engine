#ifndef RADIUMENGINE_PLUGINMANAGER_HPP
#define RADIUMENGINE_PLUGINMANAGER_HPP

#include <vector>
#include <memory>

#include <QObject>
#include <QObjectList>

#include <Engine/Plugins/PluginInterface.hpp>

namespace Ra {
namespace Engine {
namespace Plugin {

/// TYPEDEF
typedef std::shared_ptr< PluginInterface > PluginPointer;
typedef std::vector< PluginPointer > PluginVector;

class PluginManager : public QObject {
    /// MACRO
    Q_OBJECT
public:
    /// ENUM
    enum class PluginType {
        STATIC,
        DYNAMIC
    };

    /// CONSTRUCTOR
    PluginManager( const std::string& dynamic_plugin_path, QObject* parent = nullptr ) : QObject( parent ) { }

    /// DESTRUCTOR
    ~PluginManager() { }

    /// PLUGIN
    void loadPlugins( const std::string& path );
    void loadStaticPlugins();
    void loadDynamicPlugins( const std::string& path );

    inline PluginVector getStaticPlugins()  const { return m_staticPlugin;  }
    inline PluginVector getDynamicPlugins() const { return m_dynamicPlugin; }

protected:
    /// VARIABLE
    PluginVector m_staticPlugin;
    PluginVector m_dynamicPlugin;
};

} // namespace Plugin
} // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_PLUGINMANAGER_HPP
