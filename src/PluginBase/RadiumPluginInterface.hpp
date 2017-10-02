#ifndef RADIUM_RADIUMPLUGININTERFACE_HPP
#define RADIUM_RADIUMPLUGININTERFACE_HPP

#include <vector>
#include <memory>

class QWidget;
class QMenu;
class QAction;

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
    namespace Gui
    {
        class FeaturePickingManager;
    }

    namespace Engine
    {
        class Renderer;
    }

    /// Data passed to the plugin constructor.
    struct PluginContext
    {
        Engine::RadiumEngine* m_engine;
        GuiBase::SelectionManager* m_selectionManager;
        Gui::FeaturePickingManager* m_featureManager;
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

            /**
             * @brief Tells whether the plugin wants to add a feature widget
             * (inside the UI tab widget dedicated to feature tracking) or not.
             * If it does, getWidget() will be called.
             * @return True if the plugin wants to add a widget, false otherwise
             */
            virtual bool doAddFeatureTrackingWidget() { return false; }

            /**
             * @brief Creates the feature widget to be added to the ui and then returns it.
             * If connections are needed (between plugin ui and plugin internals)
             * they have to be created here.
             * @return The created and configured feature widget
             */
            virtual QWidget* getFeatureTrackingWidget() { return nullptr; }

            virtual bool doAddRenderer() { return false; }

            /**
             * @brief addRenderers
             *
             * \param rds the TODO
             * \warning Allocated renderers are given to the application and
             * SHOULD not be destroyed by the plugin
             */
            virtual void addRenderers(std::vector<std::shared_ptr<Engine::Renderer>> */*rds*/) {}
        };
    }
}

#define RadiumPluginInterface_IID "RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )


#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
