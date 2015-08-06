#include <Engine/Plugins/PluginManager.hpp>

#include <QDir>
#include <QPluginLoader>

namespace Ra
{
    namespace Engine
    {
        namespace Plugin
        {

            void PluginManager::loadPlugins ( const std::string& path )
            {
                loadStaticPlugins();
                loadDynamicPlugins ( path );
            }

            void PluginManager::loadStaticPlugins()
            {
                QObjectList list = QPluginLoader::staticInstances();
                for ( auto object : list )
                {
                    PluginPointer plugin = qobject_cast<PluginPointer> ( object );
                    if ( plugin != nullptr )
                    {
                        m_staticPlugin.push_back ( plugin );
                    }
                    else
                    {
                        // WARNING
                    }
                }
            }

            void PluginManager::loadDynamicPlugins ( const std::string& path )
            {
                QDir pluginsDir ( QString ( path ) );
                QObjectList list;

                foreach ( QString fileName, pluginsDir.entryList ( QDir::Files ) )
                {
                    QPluginLoader loader ( pluginsDir.absoluteFilePath ( fileName ) );
                    QObject* object = loader.instance();
                    if ( !object )
                    {
                        continue;
                    }
                    list.append ( plugin );
                }

                for ( auto object : list )
                {
                    PluginPointer plugin = qobject_cast<PluginPointer> ( object );
                    if ( plugin != nullptr )
                    {
                        m_dynamicPlugin.push_back ( plugin );
                    }
                    else
                    {
                        // WARNING
                    }
                }
            }

        } // namespace Plugin
    } // namespace Engine
} // namespace Ra
