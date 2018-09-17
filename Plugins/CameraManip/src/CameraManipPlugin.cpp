#include <CameraManipPlugin.hpp>

#include <fstream>

#include <QAction>
#include <QFileDialog>
#include <QIcon>
#include <QSettings>
#include <QToolBar>

#include <Engine/ItemModel/ItemEntry.hpp>
#include <Engine/Managers/CameraManager/CameraManager.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/Viewer/TrackballCamera.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

#include "ui_CameraManipUI.h"
#include <UI/CameraManipUI.h>

namespace CameraManipPlugin {

CameraManipPluginC::CameraManipPluginC() : m_selectionManager( nullptr ) {}

CameraManipPluginC::~CameraManipPluginC() {}

void CameraManipPluginC::registerPlugin( const Ra::PluginContext& context ) {
    // register selection context
    m_selectionManager = context.m_selectionManager;
    m_viewer = context.m_viewer;
    connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this,
             &CameraManipPluginC::onCurrentChanged );
}

bool CameraManipPluginC::doAddWidget( QString& name ) {
    name = "CameraManip";
    return true;
}

QWidget* CameraManipPluginC::getWidget() {
    m_widget = new CameraManipUI();
    connect( m_widget->ui->m_useCamera, &QPushButton::clicked, this,
             &CameraManipPluginC::useSelectedCamera );
    connect( m_widget->ui->m_saveCamera, &QPushButton::clicked, this,
             &CameraManipPluginC::saveCamera );
    connect( m_widget->ui->m_loadCamera, &QPushButton::clicked, this,
             &CameraManipPluginC::loadCamera );
    return m_widget;
}

bool CameraManipPluginC::doAddMenu() {
    return false;
}

QMenu* CameraManipPluginC::getMenu() {
    return nullptr;
}

bool CameraManipPluginC::doAddAction( int& nb ) {
    nb = 0;
    return false;
}

QAction* CameraManipPluginC::getAction( int id ) {
    return nullptr;
}

void CameraManipPluginC::useSelectedCamera() {
    if ( m_selectionManager->hasSelection() )
    {
        const Ra::Engine::ItemEntry& ent = m_selectionManager->currentItem();
        Ra::Engine::Camera* camera = dynamic_cast<Ra::Engine::Camera*>( ent.m_component );
        if ( camera )
        {
            m_viewer->getCameraInterface()->getCamera()->show( true );
            m_viewer->getCameraInterface()->setCamera( camera );
        }
    }
}

void CameraManipPluginC::saveCamera() {
    QSettings settings;
    QString path = settings.value( "CameraManip::camera_file", QDir::homePath() ).toString();
    path = QFileDialog::getSaveFileName( nullptr, "Open Camera", path, "*.cam" );
    if ( path.size() == 0 )
    {
        return;
    }
    settings.setValue( "CameraManip::camera_file", path );

    std::ofstream outFile( path.toStdString() );
    if ( !outFile.is_open() )
    {
        LOG( logWARNING ) << "Could not open file to save the camera: " << path.toStdString();
        return;
    }

    auto manip = static_cast<Ra::Gui::TrackballCamera*>( m_viewer->getCameraInterface() );
    auto camera = manip->getCamera();
    outFile << "#Radium_camera_state" << std::endl;
    outFile << (int)camera->getType() << std::endl;
    outFile << camera->getFrame().matrix() << std::endl;
    outFile << std::endl;
    outFile << camera->getFOV() << " " << camera->getZNear() << " " << camera->getZFar() << " "
            << camera->getZoomFactor() << " " << camera->getAspect() << std::endl;
    outFile << std::endl;
    outFile << "#Radium_camera_manip_state" << std::endl;
    outFile << manip->getTrackballCenter().transpose() << " " << manip->getTrackballRadius()
            << std::endl;
}

void CameraManipPluginC::loadCamera() {
    QSettings settings;
    QString path = settings.value( "CameraManip::camera_file", QDir::homePath() ).toString();
    path = QFileDialog::getOpenFileName( nullptr, "Open Camera", path, "*.cam" );
    if ( path.size() == 0 )
    {
        return;
    }
    settings.setValue( "CameraManip::camera_file", path );

    std::ifstream inFile( path.toStdString() );
    if ( !inFile.is_open() )
    {
        LOG( logWARNING ) << "Could not open file to load the camera: " << path.toStdString();
        return;
    }

    std::string str;
    int type;
    Scalar M[16];                     // 4x4 view matrix;
    Scalar fov, znear, zfar, zoom, a; // camera data
    Scalar x, y, z, d;                // manip data

    inFile >> str;
    bool result = !inFile.fail();
    inFile >> type;
    result = !inFile.fail();
    for ( uint i = 0; i < 16; ++i )
    {
        inFile >> M[i];
        result &= !inFile.fail();
    }
    inFile >> fov >> znear >> zfar >> zoom >> a >> str >> x >> y >> z >> d;
    result &= !inFile.fail();

    if ( !result )
    {
        LOG( logWARNING ) << "Could not load data from camera file: " << path.toStdString();
        return;
    }

    auto manip = static_cast<Ra::Gui::TrackballCamera*>( m_viewer->getCameraInterface() );
    auto camera = manip->getCamera();

    Ra::Core::Matrix4 frame;
    frame << M[0], M[1], M[2], M[3], M[4], M[5], M[6], M[7], M[8], M[9], M[10], M[11], M[12], M[13],
        M[14], M[15];
    Ra::Core::Transform T( frame );
    camera->setType( Ra::Engine::Camera::ProjType( type ) );
    camera->setFrame( T );
    camera->setFOV( fov );
    camera->setZNear( znear );
    camera->setZFar( zfar );
    camera->setZoomFactor( zoom );

    manip->setCamera( camera );
    manip->setTrackballRadius( d );
    manip->setTrackballCenter( Ra::Core::Vector3( x, y, z ) );
}

void CameraManipPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev ) {
    m_widget->ui->m_useCamera->setEnabled( false );
    if ( m_selectionManager->hasSelection() )
    {
        const Ra::Engine::ItemEntry& ent = m_selectionManager->currentItem();
        if ( dynamic_cast<Ra::Engine::Camera*>( ent.m_component ) )
        {
            m_widget->ui->m_useCamera->setEnabled( true );
        }
    }
}

} // namespace CameraManipPlugin
