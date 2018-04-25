#include "MeshFeatureTrackingUI.h"

#include <iostream>

#include "ui_MeshFeatureTrackingUI.h"

MeshFeatureTrackingUI::MeshFeatureTrackingUI( QWidget* parent ) :
    QFrame( parent ),
    ui( new Ui::MeshFeatureTrackingUI ) {
    ui->setupUi( this );
}

MeshFeatureTrackingUI::~MeshFeatureTrackingUI() {
    delete ui;
}

void MeshFeatureTrackingUI::setMaxV( int max ) {
    ui->m_vertexIdx->setMaximum( std::max( max - 1, 0 ) );
}

void MeshFeatureTrackingUI::setMaxT( int max ) {
    ui->m_triangleIdx->setMaximum( std::max( max - 1, 0 ) );
}

void MeshFeatureTrackingUI::updateTracking( const FeatureData& data, const Ra::Core::Math::Vector3& pos,
                                            const Ra::Core::Math::Vector3& vec ) {
    ui->m_vertexInfo->setEnabled( false );
    ui->m_edgeInfo->setEnabled( false );
    ui->m_triangleInfo->setEnabled( false );
    switch ( data.m_mode )
    {
    case Ra::Engine::Renderer::VERTEX:
    {
        ui->m_vertexInfo->setEnabled( true );
        ui->m_vertexIdx->blockSignals( true );
        ui->m_vertexIdx->setValue( data.m_data[0] );
        ui->m_vertexIdx->blockSignals( false );
        ui->m_vertexPX->setText( QString::number( pos[0] ) );
        ui->m_vertexPY->setText( QString::number( pos[1] ) );
        ui->m_vertexPZ->setText( QString::number( pos[2] ) );
        ui->m_vertexNX->setText( QString::number( vec[0] ) );
        ui->m_vertexNY->setText( QString::number( vec[1] ) );
        ui->m_vertexNZ->setText( QString::number( vec[2] ) );
        break;
    }
    case Ra::Engine::Renderer::EDGE:
    {
        ui->m_edgeInfo->setEnabled( true );
        ui->m_edgeV0->setText( QString::number( data.m_data[0] ) );
        ui->m_edgeV1->setText( QString::number( data.m_data[1] ) );
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    {
        ui->m_triangleInfo->setEnabled( true );
        ui->m_triangleIdx->blockSignals( true );
        ui->m_triangleIdx->setValue( data.m_data[3] );
        ui->m_triangleIdx->blockSignals( false );
        ui->m_triangleV0->setText( QString::number( data.m_data[0] ) );
        ui->m_triangleV1->setText( QString::number( data.m_data[1] ) );
        ui->m_triangleV2->setText( QString::number( data.m_data[2] ) );
        break;
    }
    default:
        break;
    }
}

void MeshFeatureTrackingUI::on_m_vertexIdx_valueChanged( int arg1 ) {
    emit vertexIdChanged( arg1 );
}

void MeshFeatureTrackingUI::on_m_triangleIdx_valueChanged( int arg1 ) {
    emit triangleIdChanged( arg1 );
}
