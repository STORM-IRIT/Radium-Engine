#ifndef ANIMATIONUI_H
#define ANIMATIONUI_H

#include <QFrame>

namespace UI {
class AnimationUi;
}

namespace AnimationPlugin {
class AnimationPluginC;
}

namespace Ui {
class AnimationUI;
}

/// The AnimationUI class is the widget for the AnimationPlugin.
class AnimationUI : public QFrame {
    Q_OBJECT

    friend class AnimationPlugin::AnimationPluginC;

  public:
    explicit AnimationUI( QWidget* parent = 0 );
    ~AnimationUI();

  signals:
    /// Emitted by on_m_xray_clicked.
    void toggleXray( bool );

    /// Emitted by on_m_showSkeleton_toggled.
    void showSkeleton( bool );

    /// Emitted by on_m_play_clicked.
    void play();

    /// Emitted by on_m_play_clicked.
    void pause();

    /// Emitted by on_m_step_clicked.
    void step();

    /// Emitted by on_m_reset_clicked.
    void stop();

    /// Emitted by on_m_animationID_valueChanged.
    void animationID( int );

    /// Emitted by on_m_timeStep_currentIndexChanged.
    void toggleAnimationTimeStep( bool );

    /// Emitted by on_m_speed_valueChanged.
    void animationSpeed( double );

    /// Emitted by for the user asking for slow motion.
    void toggleSlowMotion( bool );

  private slots:
    /// Slot for the user activating xray display of bones.
    void on_m_xray_clicked( bool checked );

    /// Slot for the user activating display of bones.
    void on_m_showSkeleton_toggled( bool checked );

    /// Slot for the user asking to play/pause the animation.
    void on_m_play_clicked( bool checked );

    /// Slot for the user asking to step the animation once.
    void on_m_step_clicked();

    /// Slot for the user asking to reset the animation.
    void on_m_reset_clicked();

    /// Slot for the user changing the animation to play.
    void on_m_animationID_valueChanged( int arg1 );

    /// Slot for the user asking to use the animation timestep or the application's.
    void on_m_timeStep_currentIndexChanged( int index );

    /// Slot for the user changing the animation speed.
    void on_m_speed_valueChanged( double arg1 );

    /// Slot for the user asking for slow motion.
    void on_m_slowMo_toggled( bool checked );

  private:
    /// The actual widget UI.
    Ui::AnimationUI* ui;

    /// Updates the displayed animation time.
    void updateTime( float t );
};

#endif // ANIMATIONUI_H
