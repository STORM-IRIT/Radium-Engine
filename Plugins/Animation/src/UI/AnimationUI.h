#ifndef ANIMATIONUI_H
#define ANIMATIONUI_H

#include <QFrame>

namespace Ui {
class AnimationUI;
}

class AnimationUI : public QFrame
{
    Q_OBJECT

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

private:
    Ui::AnimationUI *ui;
};

#endif // ANIMATIONUI_H
