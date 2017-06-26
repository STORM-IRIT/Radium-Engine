#include <PbrtLoaderPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Assets/PbrtFileLoader.hpp>

namespace PbrtLoaderPlugin
{
    PbrtLoaderPluginC::~PbrtLoaderPluginC()
    {

    }

    void PbrtLoaderPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        Ra::Asset::PbrtFileLoader * pbrtFileLoader = new Ra::Asset::PbrtFileLoader();

        context.m_engine->registerFileLoader( pbrtFileLoader );
    }

    bool PbrtLoaderPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* PbrtLoaderPluginC::getWidget()
    {
        return nullptr;
    }

    bool PbrtLoaderPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* PbrtLoaderPluginC::getMenu()
    {
        return nullptr;
    }

    bool PbrtLoaderPluginC::doAddAction( int& nb )
    {
        nb = 0;
        return false;
    }

    QAction* PbrtLoaderPluginC::getAction( int id )
    {
        return nullptr;
    }
}
