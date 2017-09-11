#include "MeshFeatureTrackingUI.h"

#include <iostream>

#include "ui_MeshFeatureTrackingUI.h"

MeshFeatureTrackingUI::MeshFeatureTrackingUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MeshFeatureTrackingUI)
{
    ui->setupUi(this);
    this->setVisible( false );
}

MeshFeatureTrackingUI::~MeshFeatureTrackingUI()
{
    delete ui;
}

void MeshFeatureTrackingUI::updateTracking( const Ra::Gui::FeatureData& data,
                                            const Ra::Core::Vector3 &pos,
                                            const Ra::Core::Vector3 &vec )
{
    this->setVisible( false );
    ui->m_vertexInfo->setVisible(false);
    ui->m_edgeInfo->setVisible(false);
    ui->m_triangleInfo->setVisible(false);
    switch (data.m_featureType)
    {
    case Ra::Engine::Renderer::VERTEX:
    {
        this->setVisible( true );
        ui->m_vertexInfo->setVisible(true);
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
        this->setVisible( true );
        ui->m_edgeInfo->setVisible(true);
        ui->m_edgeV0->setText( QString::number( data.m_data[0] ) );
        ui->m_edgeV1->setText( QString::number( data.m_data[1] ) );
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    {
        this->setVisible( true );
        ui->m_triangleInfo->setVisible(true);
        ui->m_triangleV0->setText( QString::number( data.m_data[1] ) );
        ui->m_triangleV1->setText( QString::number( data.m_data[2] ) );
        ui->m_triangleV2->setText( QString::number( data.m_data[3] ) );
        break;
    }
    default:
        break;
    }
}
