#ifndef RADIUM_RADIUMPLUGININTERFACE_HPP
#define RADIUM_RADIUMPLUGININTERFACE_HPP

#include <memory>
#include <vector>

#include <QtPlugin>

class QWidget;
class QMenu;
class QAction;
class QOpenGLContext;
class QString;

namespace Ra {
namespace Engine {
class RadiumEngine;
}
namespace GuiBase {
class SelectionManager;
}
namespace Gui {
class PickingManager;
}

namespace Engine {
class Renderer;
}

namespace Asset {
class FileLoaderInterface;
}

/// Data passed to the plugin constructor.
struct PluginContext {
    Engine::RadiumEngine* m_engine;
    GuiBase::SelectionManager* m_selectionManager;
    Gui::PickingManager* m_pickingManager;
};

namespace Plugins {
class RadiumPluginInterface {
  public:
    virtual ~RadiumPluginInterface() {}

    /**
     * @brief Pass arguments for plugin initialization.
     * This method is supposed to create the system (<emph>new</emph> it)
     * and then call Ra::Engine::RadiumEngine::registerSystem().
     * Without doing this, your system won't do anything
     * (unless you just want to add UI elements)
     * @param context : plugin context containing the engine and UI interfaces.
     */
    virtual void registerPlugin( const PluginContext& context ) = 0;

    /**
     * @brief Tells wether the plugin wants to add a widget
     * (inside the UI tab widget) or not. If it does, it must provide
     * a name for it, and getWidget() will be called.
     * @param name Name of the tab to be added, if needed.
     * @return True if the plugin wants to add a widget, false otherwise
     */
    virtual bool doAddWidget( QString& name ) = 0;

    /**
     * @brief Creates the widget to be added to the ui and then returns it.
     * If connections are needed (between plugin ui and plugin internals)
     * they have to be created here.
     * @return The created and configured widget
     * @todo Find a better name for this ?
     */
    virtual QWidget* getWidget() = 0;

    /**
     * @brief Tells wether the plugin wants to add a menu
     * or not. If it does, getMenu() will be called.
     * @return True if the plugin wants to add a menu, false otherwise
     */
    virtual bool doAddMenu() = 0;

    /**
     * @brief Creates to menu to be added to the ui and then returns it.
     * @return The created menu
     * @todo Find a better name ?
     */
    virtual QMenu* getMenu() = 0;

    /**
     * @brief Tells wether the plugin wants to add actions
     * or not. If it does, getAction() will be called for each one of them.
     * @param nb the number of action the plugin wants to add.
     * @return True if the plugin wants to add a menu, false otherwise
     */
    virtual bool doAddAction( int& nb ) = 0;

    /**
     * @brief Returns the action to be added to the ui and then returns it.
     * @return The action to add.
     */
    virtual QAction* getAction( int id ) = 0;

    virtual bool doAddRenderer() { return false; }

    /**
     * @brief addRenderers
     *
     * \param rds the TODO
     * \warning Allocated renderers are given to the application and
     * SHOULD not be destroyed by the plugin
     */
    virtual void addRenderers( std::vector<std::shared_ptr<Engine::Renderer>>* /*rds*/ ) {}

    virtual bool doAddFileLoader() { return false; }

    virtual void
    addFileLoaders( std::vector<std::shared_ptr<Asset::FileLoaderInterface>>* /*fl*/ ) {}

    /**
     * @brief openGlInitialize
     *
     * \param context (const PluginContext& context) the Plugin context
     * \param openGLContext (const QOpenGLContext *openGLContext) the existing OpenGLcontext use by
     * the viewer
     * @see https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/ for the
     * reason a const QOpenGLContext * is given \warning Allocated renderers are given to the
     * application and SHOULD not be destroyed by the plugin
     */
    virtual void openGlInitialize( const PluginContext& context,
                                   const QOpenGLContext* openGLContext ) {}

    virtual bool doAddROpenGLInitializer() { return false; }
};
} // namespace Plugins
} // namespace Ra

#define RadiumPluginInterface_IID "RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )

#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
