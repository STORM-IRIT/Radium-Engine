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
namespace Core {
namespace Asset {
class FileLoaderInterface;
} // namespace Asset
} // namespace Core

namespace Engine {
class RadiumEngine;
class Renderer;
} // namespace Engine

namespace GuiBase {
class SelectionManager;
} // namespace GuiBase

namespace Gui {
class PickingManager;
class Viewer;
} // namespace Gui

/**
 * Data passed to the Plugin constructor.
 */
struct PluginContext {
    Engine::RadiumEngine* m_engine = nullptr;
    GuiBase::SelectionManager* m_selectionManager = nullptr;
    Gui::PickingManager* m_pickingManager = nullptr;
    Gui::Viewer* m_viewer = nullptr;
    std::string m_exportDir = ".";
};

namespace Plugins {
class RadiumPluginInterface {
  public:
    virtual ~RadiumPluginInterface() {}

    /**
     * \brief Pass arguments for plugin initialization.
     *
     * This method must create and register all the services that the plugin
     * wants to offer: System, Renderers, Materials, gui...
     *
     * \param context the plugin context containing the Engine and gui interfaces.
     */
    virtual void registerPlugin( const PluginContext& context ) = 0;

    /**
     * \brief Tells wether the plugin wants to add a widget (inside the gui
     *        tab widget) or not. If it does, it must provide a name for it,
     *        and getWidget() will be called.
     *
     * \param name Name of the tab to be added, if needed.
     * \return True if the plugin wants to add a widget, false otherwise.
     */
    virtual bool doAddWidget( QString& name ) = 0;

    /**
     * \brief Creates the widget to be added to the ui and then returns it.
     *
     * If connections are needed (between plugin ui and plugin internals)
     * they have to be created here.
     * \return The created and configured widget.
     * \todo Find a better name for this ?
     */
    virtual QWidget* getWidget() = 0;

    /**
     * \brief Tells wether the plugin wants to add a menu or not.
     *        If it does, getMenu() will be called.
     * \return True if the plugin wants to add a menu, false otherwise
     */
    virtual bool doAddMenu() = 0;

    /**
     * \brief Creates the menu to be added to the ui and then returns it.
     * \return The created menu.
     * \todo Find a better name ?
     */
    virtual QMenu* getMenu() = 0;

    /**
     * \brief Tells wether the plugin wants to add actions or not.
     *        If it does, getAction() will be called for each one of them.
     * \param nb the number of action the plugin wants to add.
     * \return True if the plugin wants to add a menu, false otherwise.
     */
    virtual bool doAddAction( int& nb ) = 0;

    /**
     * \brief Returns the action to be added to the ui and then returns it.
     * \return The action to add.
     */
    virtual QAction* getAction( int id ) = 0;

    /**
     * \brief Tells wether the plugin will add Renderers or not.
     *        If it does, addRenderers() will be called for all of them at once.
     * \return true if the plugin offer rendering services, false otherwise.
     */
    virtual bool doAddRenderer() { return false; }

    /**
     * \brief Creates the additional Renderers brought by the plugin.
     * \param rds the renderers that the plugin wants to expose to the application.
     * \warning Allocated renderers are given to the application that takes ownership.
     *          They MUST not be destroyed by the plugin.
     */
    virtual void addRenderers( std::vector<std::shared_ptr<Engine::Renderer>>* rds ) {}

    /**
     * \brief Tells whether the Plugin will add file loaders or not.
     *        If it does, addFileLoaders() will be called for all of them at once.
     * \return true if the plugin offer file loading services, false otherwise.
     */
    virtual bool doAddFileLoader() { return false; }

    /**
     * \brief Creates the additional file loader brought by the plugin.
     * \param fl the file loaders that the plugin wants to expose to the application.
     */
    virtual void
    addFileLoaders( std::vector<std::shared_ptr<Core::Asset::FileLoaderInterface>>* fl ) {}

    /**
     * \brief Tells whether the plugin offers OpenGL based services, that need
     *        to be initialized after the OpenGL context is created, or not.
     *        If it does, openGlInitialize() will be called for all of them at once.
     * \return true if plugin offers OppenGL services, false otherwise.
     */
    virtual bool doAddROpenGLInitializer() { return false; }

    /**
     * \brief openGlInitialize
     *
     * \param context (const PluginContext& context) the Plugin context
     * \see https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/
     *      for the reason a `const QOpenGLContext *` is given.
     * \warning Allocated renderers are given to the application and MUST not be
     *          destroyed by the plugin.
     * \note Plugins that want to manage OpenGL resources MUST ENSURE at the time
     *       they create/access these resources that the current openGLContext
     *       is the one pass when initializing OpenGL here and stored in the PluginContext...
     * \note When this function is called from the application, The active OpenGL
     *       Context is the one that will be used for rendering.
     *
     * Developers of plugins that need to manage OpenGL resources in a direct way
     * (recommended for plugins that manage OpenGL resources for rendering or
     * interaction with rendering) must implement this method and memorize the
     * viewer of the PluginContext.
     * Each time they need to create or destroy an OpenGL resource, they must
     * activate this context (viewer->makeCurrent()) and realease it after
     * usage (viewer->doneCurrent()).
     * \see Ra::Gui::Viewer.
     */
    virtual void openGlInitialize( const PluginContext& context ) {}
};
} // namespace Plugins
} // namespace Ra

#define RadiumPluginInterface_IID "RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )

#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
