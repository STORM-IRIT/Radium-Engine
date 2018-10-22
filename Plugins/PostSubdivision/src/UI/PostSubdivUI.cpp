#include "PostSubdivUI.h"

#include <iostream>

#include <QColorDialog>

#include "ui_PostSubdivUI.h"

PostSubdivUI::PostSubdivUI( QWidget* parent ) : QFrame( parent ), ui( new Ui::PostSubdivUI ) {
    ui->setupUi( this );
}

PostSubdivUI::~PostSubdivUI() {
    delete ui;
}

void PostSubdivUI::on_m_subdivMethod_currentIndexChanged( int index ) {
    emit subdivMethodChanged( index );
}

void PostSubdivUI::on_m_subdivIter_valueChanged( int arg1 ) {
    emit subdivIterChanged( arg1 );
}
