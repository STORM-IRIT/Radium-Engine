#ifndef ANIMATIONUI_H
#define ANIMATIONUI_H

#include <QFrame>

namespace UI
{
    class AnimationUi;
}

namespace AnimationPlugin
{
    class AnimationPluginC;
}

namespace Ui {
class AnimationUI;
}

class AnimationUI : public QFrame
{
    Q_OBJECT

    friend class AnimationPlugin::AnimationPluginC;

public:
    explicit AnimationUI(QWidget *parent = 0);
    ~AnimationUI();

signals:
    void toggleXray(bool);
    void showSkeleton(bool);
    void play();
    void pause();
    void step();
    void stop();
    void animationID(int);
    void toggleAnimationTimeStep(bool);
    void animationSpeed(double);
    void toggleSlowMotion(bool);
    void cacheFrame();
    void restoreFrame(int);

private slots:
    void on_m_xray_clicked(bool checked);
    void on_m_showSkeleton_toggled(bool checked);
    void on_m_play_clicked(bool checked);
    void on_m_step_clicked();
    void on_m_reset_clicked();
    void on_m_animationID_valueChanged(int arg1);
    void on_m_timeStep_currentIndexChanged(int index);
    void on_m_speed_valueChanged(double arg1);
    void on_m_slowMo_toggled(bool checked);
    void on_m_cacheFrame_clicked();
    void on_m_restoreFrame_clicked();
    void frameLoaded(int f);

private:
    Ui::AnimationUI *ui;

    void updateTime( float t);
    void updateFrame( int f );
};

#endif // ANIMATIONUI_H
