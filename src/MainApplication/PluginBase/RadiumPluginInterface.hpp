#ifndef RADIUM_RADIUMPLUGININTERFACE_HPP
#define RADIUM_RADIUMPLUGININTERFACE_HPP

class QWidget;
class QMenu;

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
    namespace GuiBase
    {
        class SelectionManager;
    }

    /// Data passed to the plugin constructor.
    struct PluginContext
    {
        Engine::RadiumEngine* m_engine;
        GuiBase::SelectionManager* m_selectionManager;
    };

    namespace Plugins
    {
        class RadiumPluginInterface
        {
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
             * or not. If it does, it must provide
             * a name for it, and getMenu() will be called.
             * @return True if the plugin wants to add a menu, false otherwise
             */
            virtual bool doAddMenu() = 0;

            /**
             * @brief Creates to menu to be added to the ui and then returns it.
             * @return The created menu
             * @todo Find a better name ?
             */
            virtual QMenu* getMenu() = 0;
        };
    }
}

#define RadiumPluginInterface_IID "RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )


#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
