#include "MeshPaintUI.h"

#include <iostream>

#include <QColorDialog>

#include "ui_MeshPaintUI.h"

MeshPaintUI::MeshPaintUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MeshPaintUI)
{
    ui->setupUi( this );
}

MeshPaintUI::~MeshPaintUI()
{
    delete ui;
}

void MeshPaintUI::on_changeColor_pb_clicked()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid())
    {
        ui->changeColor_pb->setPalette( QPalette(color) );
        emit colorChanged( color );
    }
}

void MeshPaintUI::on_paintColor_rb_toggled( bool checked )
{
    ui->changeColor_pb->setEnabled( checked );
    emit paintColor( checked );
}
