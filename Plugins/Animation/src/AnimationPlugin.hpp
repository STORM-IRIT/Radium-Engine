#ifndef ANIMATIONPLUGIN_HPP_
#define ANIMATIONPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QObject>
#include <QtPlugin>

#include <UI/AnimationUI.h>

#include <AnimationPluginMacros.hpp>

namespace Ra {
namespace Engine {
class RadiumEngine;
} // namespace Engine
} // namespace Ra

/// The AnimationPlugin manages skeleton-based character animation.
namespace AnimationPlugin {
// Due to an ambigous name while compiling with Clang, we must differentiate the
// plugin class from plugin namespace
class AnimationPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    AnimationPluginC();
    ~AnimationPluginC();

    void registerPlugin( const Ra::PluginContext& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  public slots:
    /// Slot for the user activating xray display of bones.
    void toggleXray( bool on );

    /// Slot for the user activating display of bones.
    void toggleSkeleton( bool on );

    /// Slot for the user asking to step the animation once.
    void step();

    /// Slot for the user asking to play the animation.
    void play();

    /// Slot for the user asking to pause the animation.
    void pause();

    /// Slot for the user asking to reset the animation.
    void reset();

    /// Slot for the user changing the animation to play.
    void setAnimation( uint i );

    /// Slot for the user asking to use the animation timestep or the application's.
    void toggleAnimationTimeStep( bool status );

    /// Slot for the user changing the animation speed.
    void setAnimationSpeed( Scalar value );

    /// Slot for the user asking for slow motion.
    void toggleSlowMotion( bool status );

    /// Updates the displayed animation time.
    void updateAnimTime();

    /// Save all animation data to a file (one per component).
    void cacheFrame();

    /// Restore all animation from a file, if such a file exists.
    void restoreFrame( int frame );

    /// Request changing the data file directory.
    void changeDataDir();

  private:
    /// The data directory.
    std::string m_dataDir;

    /// The AnimationSystem.
    class AnimationSystem* m_system;

    /// The Animation widget.
    AnimationUI* m_widget;

    /// The SelectionManager of the Viewer.
    Ra::GuiBase::SelectionManager* m_selectionManager;
};

} // namespace AnimationPlugin

#endif // ANIMATIONPLUGIN_HPP_
