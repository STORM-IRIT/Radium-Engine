#include "AnimationUI.h"
#include "ui_AnimationUI.h"

#include <iostream>

AnimationUI::AnimationUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AnimationUI)
{
    ui->setupUi(this);
    ui->m_play->setProperty( "pressed", false );
    ui->m_play->style()->unpolish( ui->m_play );
    ui->m_play->style()->polish( ui->m_play );
    ui->m_play->update();
}

AnimationUI::~AnimationUI()
{
    delete ui;
}

void AnimationUI::on_m_xray_clicked( bool checked ) {
    emit toggleXray(checked);
}

void AnimationUI::on_m_showSkeleton_toggled( bool checked ) {
    emit showSkeleton(checked);
}

void AnimationUI::on_m_play_clicked( bool checked ) {
    if( checked ) {
        ui->m_play->setChecked( true );

        ui->m_play->setProperty( "pressed", true );
        ui->m_play->style()->unpolish( ui->m_play );
        ui->m_play->style()->polish( ui->m_play );
        ui->m_play->update();

        emit play();
    } else {
        ui->m_play->setChecked( false );

        ui->m_play->setProperty( "pressed", false );
        ui->m_play->style()->unpolish( ui->m_play );
        ui->m_play->style()->polish( ui->m_play );
        ui->m_play->update();

        emit pause();
    }
}

void AnimationUI::on_m_step_clicked() {
    ui->m_play->setChecked( false );

    ui->m_play->setProperty( "pressed", false );
    ui->m_play->style()->unpolish( ui->m_play );
    ui->m_play->style()->polish( ui->m_play );
    ui->m_play->update();

    emit step();
}

void AnimationUI::on_m_reset_clicked() {
    ui->m_play->setChecked( false );

    ui->m_play->setProperty( "pressed", false );
    ui->m_play->style()->unpolish( ui->m_play );
    ui->m_play->style()->polish( ui->m_play );
    ui->m_play->update();

    emit stop();
}

void AnimationUI::on_m_animationID_valueChanged( int arg1 ) {
    emit animationID( arg1 );
}

void AnimationUI::on_m_timeStep_currentIndexChanged( int index ) {
    emit toggleAnimationTimeStep( ( index == 0 ) );
}

void AnimationUI::on_m_speed_valueChanged( double arg1 ) {
    emit animationSpeed( arg1 );
}

void AnimationUI::on_m_slowMo_toggled( bool checked ) {
    emit toggleSlowMotion( checked );
}

