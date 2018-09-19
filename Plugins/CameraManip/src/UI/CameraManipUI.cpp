#include "CameraManipUI.h"

#include "ui_CameraManipUI.h"

CameraManipUI::CameraManipUI( QWidget* parent ) : QFrame( parent ), ui( new Ui::CameraManipUI ) {
    ui->setupUi( this );
}

CameraManipUI::~CameraManipUI() {
    delete ui;
}
