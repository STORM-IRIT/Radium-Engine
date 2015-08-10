#include <MainApplication/Gui/MainWindow.hpp>

#include <QFileDialog>
#include <QMouseEvent>

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>

namespace Ra
{

    Gui::MainWindow::MainWindow( QWidget* parent )
        : QMainWindow( parent )
    {
        setupUi( this );

        setWindowIcon( QPixmap( "../Assets/Images/RadiumIcon.png" ) );
        setWindowTitle( QString( "Radium Engine" ) );

        QStringList headers;
        headers << tr( "Entities -> Components" );
        m_entityTreeModel = new EntityTreeModel( headers );
        m_entitiesTreeView->setModel( m_entityTreeModel );

        createConnections();

        m_viewer->getCamera()->resetCamera();
    }

    Gui::MainWindow::~MainWindow()
    {
        // Child QObjects will automatically be deleted
    }

    void Gui::MainWindow::createConnections()
    {
        connect( actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile );
        connect( actionReload_Shaders, &QAction::triggered, m_viewer, &Gui::Viewer::reloadShaders );

        connect( this, SIGNAL( fileLoading( QString ) ), qApp, SLOT( loadFile( QString ) ) );
        connect( this, SIGNAL( entitiesUpdated( const std::vector<Engine::Entity*>& ) ),
                 m_entityTreeModel, SLOT( entitiesUpdated( const std::vector<Engine::Entity*>& ) ) );

        connect( m_entityTreeModel, SIGNAL( objectNameChanged( QString ) ),
                 this, SLOT( objectNameChanged( QString ) ) );
        connect( m_entityTreeModel, SIGNAL( dataChanged( QModelIndex, QModelIndex, QVector<int> ) ),
                 m_entityTreeModel, SLOT( handleRename( QModelIndex, QModelIndex, QVector<int> ) ) );

        connect( m_entitiesTreeView, SIGNAL( clicked( QModelIndex ) ), m_entityTreeModel,
                 SLOT( handleSelect( QModelIndex ) ) );

        connect( m_cameraResetButton, SIGNAL( released() ),
                 m_viewer->getCamera(), SLOT( resetCamera() ) );

        connect( m_setCameraPositionButton, SIGNAL( released() ),
                 this, SLOT( setCameraPosition() ) );
        connect( m_setCameraTargetButton, SIGNAL( released() ),
                 this, SLOT( setCameraTarget() ) );

        connect( this, SIGNAL( setCameraPosition( const Core::Vector3& ) ),
                 m_viewer->getCamera(), SLOT( setCameraPosition( const Core::Vector3& ) ) );
        connect( this, SIGNAL( setCameraTarget( const Core::Vector3& ) ),
                 m_viewer->getCamera(), SLOT( setCameraTarget( const Core::Vector3& ) ) );

        connect( m_viewer->getCamera(), SIGNAL( cameraPositionChanged( const Core::Vector3& ) ),
                 this, SLOT( cameraPositionChanged( const Core::Vector3& ) ) );
        connect( m_viewer->getCamera(), SIGNAL( cameraTargetChanged( const Core::Vector3& ) ),
                 this, SLOT( cameraTargetChanged( const Core::Vector3& ) ) );

        connect( m_cameraSensitivity, SIGNAL( valueChanged( double ) ),
                 m_viewer->getCamera(), SLOT( setCameraSensitivity( double ) ) );
    }

    void Gui::MainWindow::activated( QModelIndex index )
    {
        LOG( logDEBUG ) << "Activated item " << index.row() << " " << index.column();
    }

    void Gui::MainWindow::clicked( QModelIndex index )
    {
        LOG( logDEBUG ) << "Clicked item " << index.row() << " " << index.column();
    }

    void Gui::MainWindow::entitiesUpdated()
    {
        //emit entitiesUpdated(m_viewer->getEngine()->getEntities());
    }

    void Gui::MainWindow::cameraPositionChanged( const Core::Vector3& p )
    {
        m_cameraPositionX->setValue( p.x() );
        m_cameraPositionY->setValue( p.y() );
        m_cameraPositionZ->setValue( p.z() );
    }

    void Gui::MainWindow::cameraTargetChanged( const Core::Vector3& p )
    {
        m_cameraTargetX->setValue( p.x() );
        m_cameraTargetY->setValue( p.y() );
        m_cameraTargetZ->setValue( p.z() );
    }

    void Gui::MainWindow::setCameraPosition()
    {
        Core::Vector3 P( m_cameraPositionX->value(),
                         m_cameraPositionY->value(),
                         m_cameraPositionZ->value() );
        emit setCameraPosition( P );
    }

    void Gui::MainWindow::setCameraTarget()
    {
        Core::Vector3 T( m_cameraTargetX->value(),
                         m_cameraTargetY->value(),
                         m_cameraTargetZ->value() );
        emit setCameraTarget( T );
    }

    void Gui::MainWindow::loadFile()
    {
        QString path = QFileDialog::getOpenFileName( this, QString(), ".." );
        if ( path.size() > 0 )
        {
            emit fileLoading( path );
        }
    }

    void Gui::MainWindow::keyPressEvent( QKeyEvent* event )
    {
        QMainWindow::keyPressEvent( event );
        m_keyEvents.push_back( keyEventQtToRadium( event ) );
    }

    void Gui::MainWindow::keyReleaseEvent( QKeyEvent* event )
    {
        QMainWindow::keyReleaseEvent( event );
        m_keyEvents.push_back( keyEventQtToRadium( event ) );
    }

    Core::MouseEvent Gui::MainWindow::wheelEventQtToRadium( const QWheelEvent* qtEvent )
    {
        Core::MouseEvent raEvent;
        raEvent.wheelDelta = qtEvent->delta();
        if ( qtEvent->modifiers().testFlag( Qt::ControlModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::ShiftModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::AltModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
        }

        raEvent.absoluteXPosition = qtEvent->x();
        raEvent.absoluteYPosition = qtEvent->y();
        return raEvent;
    }

    Core::MouseEvent Gui::MainWindow::mouseEventQtToRadium( const QMouseEvent* qtEvent )
    {
        Core::MouseEvent raEvent;
        switch ( qtEvent->button() )
        {
            case Qt::LeftButton:
            {
                raEvent.button = Core::MouseButton::RA_MOUSE_LEFT_BUTTON;
            }
            break;

            case Qt::MiddleButton:
            {
                raEvent.button = Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON;
            }
            break;

            case Qt::RightButton:
            {
                raEvent.button = Core::MouseButton::RA_MOUSE_RIGHT_BUTTON;
            }
            break;

            default:
            {
            } break;
        }

        raEvent.modifier = 0;

        if ( qtEvent->modifiers().testFlag( Qt::ControlModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::ShiftModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::AltModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
        }

        raEvent.absoluteXPosition = qtEvent->x();
        raEvent.absoluteYPosition = qtEvent->y();
        return raEvent;
    }

    Core::KeyEvent Gui::MainWindow::keyEventQtToRadium( const QKeyEvent* qtEvent )
    {
        Core::KeyEvent raEvent;
        raEvent.key = qtEvent->key();

        raEvent.modifier = 0;

        if ( qtEvent->modifiers().testFlag( Qt::ControlModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::ShiftModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
        }

        if ( qtEvent->modifiers().testFlag( Qt::AltModifier ) )
        {
            raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
        }
        return raEvent;
    }

    Gui::Viewer* Gui::MainWindow::getViewer()
    {
        return m_viewer;
    }

} // namespace Ra
