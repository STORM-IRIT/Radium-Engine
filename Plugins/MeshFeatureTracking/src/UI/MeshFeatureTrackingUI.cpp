#include "MeshFeatureTrackingUI.h"

#include <iostream>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

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

void MeshFeatureTrackingUI::updateTracking()
{
    this->setVisible( false );
    ui->m_vertexInfo->setVisible(false);
    ui->m_edgeInfo->setVisible(false);
    ui->m_triangleInfo->setVisible(false);
    switch (current.m_featureType)
    {
    case Ra::Engine::Renderer::VERTEX:
    {
        this->setVisible( true );
        ui->m_vertexInfo->setVisible(true);
        ui->m_vertexPX->setText( QString::number( getFeaturePosition()[0] ) );
        ui->m_vertexPY->setText( QString::number( getFeaturePosition()[1] ) );
        ui->m_vertexPZ->setText( QString::number( getFeaturePosition()[2] ) );
        ui->m_vertexNX->setText( QString::number( getFeatureVector()[0]   ) );
        ui->m_vertexNY->setText( QString::number( getFeatureVector()[1]   ) );
        ui->m_vertexNZ->setText( QString::number( getFeatureVector()[2]   ) );
        break;
    }
    case Ra::Engine::Renderer::EDGE:
    {
        this->setVisible( true );
        ui->m_edgeInfo->setVisible(true);
        ui->m_edgeV0->setText( QString::number( current.m_data[0] ) );
        ui->m_edgeV1->setText( QString::number( current.m_data[1] ) );
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    {
        this->setVisible( true );
        ui->m_triangleInfo->setVisible(true);
        ui->m_triangleV0->setText( QString::number( current.m_data[1] ) );
        ui->m_triangleV1->setText( QString::number( current.m_data[2] ) );
        ui->m_triangleV2->setText( QString::number( current.m_data[3] ) );
        break;
    }
    default:
        break;
    }
}

void MeshFeatureTrackingUI::setCurrent(const Ra::Gui::FeatureData &data )
{
    current = data;
}



Ra::Core::Vector3 MeshFeatureTrackingUI::getFeaturePosition() const
{
    if (current.m_featureType == Ra::Engine::Renderer::RO)
    {
        return Ra::Core::Vector3();
    }

    const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(current.m_roIdx)
                                                           ->getMesh()->getGeometry().m_vertices;
    Ra::Core::Vector3 P(0,0,0);
    switch (current.m_featureType)
    {
    case Ra::Engine::Renderer::VERTEX:
    {
        P = v[ current.m_data[0] ];
        break;
    }
    case Ra::Engine::Renderer::EDGE:
    {
        P = ( v[ current.m_data[0] ]
            + v[ current.m_data[1] ] ) / 2.0;
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    {
        P = ( v[ current.m_data[1] ]
            + v[ current.m_data[2] ]
            + v[ current.m_data[3] ] ) / 3.0;
        break;
    }
    default:
        break;
    }

    return P;
}

Ra::Core::Vector3 MeshFeatureTrackingUI::getFeatureVector() const
{
    if (current.m_featureType == Ra::Engine::Renderer::RO)
    {
        return Ra::Core::Vector3();
    }

    const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(current.m_roIdx)
                                                           ->getMesh()->getGeometry().m_vertices;
    const auto& n = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(current.m_roIdx)
                                                           ->getMesh()->getGeometry().m_normals;
    Ra::Core::Vector3 V(0,0,0);
    switch (current.m_featureType)
    {
    case Ra::Engine::Renderer::VERTEX:
    {
        // for vertices, the normal
        V = n[ current.m_data[0] ];
        break;
    }
    case Ra::Engine::Renderer::EDGE:
    {
        // for edges, the edge vector
        V = v[ current.m_data[0] ] - v[ current.m_data[1] ];
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    {
        // for triangles, the normal
        const Ra::Core::Vector3& p0 = v[ current.m_data[1] ];
        const Ra::Core::Vector3& p1 = v[ current.m_data[2] ];
        const Ra::Core::Vector3& p2 = v[ current.m_data[3] ];
        V = (p1-p0).cross(p2-p0).normalized();
        break;
    }
    default:
        break;
    }

    return V;
}

