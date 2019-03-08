#ifndef ANIMATIONUI_H
#define ANIMATIONUI_H

#include <QFrame>

namespace UI {
class AnimationUi;
} // namespace UI

namespace AnimationPlugin {
class AnimationPluginC;
} // namespace AnimationPlugin

namespace Ui {
class AnimationUI;
} // namespace Ui

/**
 * The AnimationUI class is the widget for the AnimationPlugin.
 */
class AnimationUI : public QFrame {
    Q_OBJECT

    friend class AnimationPlugin::AnimationPluginC;

  public:
    explicit AnimationUI( QWidget* parent = nullptr );
    ~AnimationUI();

  signals:
    /**
     * Emitted when the user toggles the "x-ray" button to activate xray display of bones.
     */
    void toggleXray( bool );

    /**
     * Emitted when the user toggles the "show" checkbox to activate display of bones.
     */
    void showSkeleton( bool );

    /**
     * Emitted when the user toggles the "play" button/action to play the animation.
     */
    void play();

    /**
     * Emitted when the user toggles the "play" button/action to pause the animation.
     */
    void pause();

    /**
     * Emitted when the user toggles the "step" button/action to step the animation.
     */
    void step();

    /**
     * Emitted when the user toggles the "stop" button/action to reset the animation.
     */
    void stop();

    /**
     * Emitted when the user changes the animation to play.
     */
    void animationID( int );

    /**
     * Emitted when the user changes the timestepping mode.
     */
    void toggleAnimationTimeStep( bool );

    /**
     * Emitted when the user changes the animation speed.
     */
    void animationSpeed( double );

    /**
     * Emitted when the user toggles the "slow motion" checkbox.
     */
    void toggleSlowMotion( bool );

    /**
     * Emitted when the user hits the "cache frame" button to save the frame data.
     */
    void cacheFrame();

    /**
     * Emitted when the user hits the "Restore frame" button to load a frame.
     */
    void restoreFrame( int );

    /** Emitted when the user changes the caching directory through the "Choose Animation Data
     *  Directory" button.
     */
    void changeDataDir();

  private slots:
    /**
     * Slot for the user activating xray display of bones.
     */
    void on_m_xray_clicked( bool checked );

    /**
     * Slot for the user activating display of bones.
     */
    void on_m_showSkeleton_toggled( bool checked );

    /**
     * Slot for the user asking to play/pause the animation.
     */
    void on_m_play_clicked( bool checked );

    /**
     * Slot for the user asking to step the animation once.
     */
    void on_m_step_clicked();

    /**
     * Slot for the user asking to reset the animation.
     */
    void on_m_reset_clicked();

    /**
     * Slot for the user changing the animation to play.
     */
    void on_m_animationID_valueChanged( int arg1 );

    /**
     * Slot for the user asking to use the animation timestep or the application's.
     */
    void on_m_timeStep_currentIndexChanged( int index );

    /**
     * Slot for the user changing the animation speed.
     */
    void on_m_speed_valueChanged( double arg1 );

    /**
     * Slot for the user asking for slow motion.
     */
    void on_m_slowMo_toggled( bool checked );

    /**
     * Slot for the user asking to cache the current frame.
     */
    void on_m_cacheFrame_clicked();

    /**
     * Slot for the user asking to load cached frame.
     */
    void on_m_restoreFrame_clicked();

    /**
     * Slot for updating UI after cached frame has been succesfully loaded.
     */
    void frameLoaded( int f );

    /**
     * Slot for the user asking for changing the cached frame directory.
     */
    void on_m_saveDir_clicked();

    /**
     * Slot for updating UI when number of frames change (e.g.\ when loading scenes).
     */
    void setMaxFrame( int f );

  private:
    /// The actual widget UI.
    Ui::AnimationUI* ui;

    /// Updates the displayed animation time.
    void updateTime( float t );

    /// Updates the displayed animation frame number.
    void updateFrame( int f );
};

#endif // ANIMATIONUI_H
