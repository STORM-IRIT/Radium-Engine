#include <AssimpLoaderPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Assets/AssimpFileLoader.hpp>

namespace AssimpLoaderPlugin
{
    AssimpLoaderPluginC::~AssimpLoaderPluginC()
    {

    }

    void AssimpLoaderPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        Ra::Asset::AssimpFileLoader * assimpFileLoader = new Ra::Asset::AssimpFileLoader();

        context.m_engine->registerFileLoader( assimpFileLoader );
    }

    bool AssimpLoaderPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* AssimpLoaderPluginC::getWidget()
    {
        return nullptr;
    }

    bool AssimpLoaderPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* AssimpLoaderPluginC::getMenu()
    {
        return nullptr;
    }

    bool AssimpLoaderPluginC::doAddAction( int& nb )
    {
        nb = 0;
        return false;
    }

    QAction* AssimpLoaderPluginC::getAction( int id )
    {
        return nullptr;
    }
}
