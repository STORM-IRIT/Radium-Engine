#include <MainApplication/Gui/MainWindow.hpp>

#include <QFileDialog>
#include <QMouseEvent>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>

namespace Ra
{

    Gui::MainWindow::MainWindow( QWidget* parent )
        : QMainWindow( parent )
    {
        // Note : at this point most of the components (including the Engine) are
        // not initialized. Listen to the "started" signal.

        setupUi( this );

        setWindowIcon( QPixmap( "../Assets/Images/RadiumIcon.png" ) );
        setWindowTitle( QString( "Radium Engine" ) );

        QStringList headers;
        headers << tr( "Entities -> Components" );
        m_entityTreeModel = new EntityTreeModel( headers );
        m_entitiesTreeView->setModel( m_entityTreeModel );

        createConnections();

        mainApp->framesCountForStatsChanged(
                    m_avgFramesCount->value() );
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

        connect( this, SIGNAL( fileLoading( QString ) ), mainApp, SLOT( loadFile( QString ) ) );
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

        /// Update entities when the engine starts.
        connect(mainApp, SIGNAL(starting()),  this, SLOT(entitiesUpdated()));

        connect( m_avgFramesCount, SIGNAL( valueChanged(int) ), mainApp , SLOT( framesCountForStatsChanged( int ) ) );
        connect( mainApp , SIGNAL( updateFrameStats( const std::vector<FrameTimerData>& ) ),
                 this, SLOT( updateFramestats( const std::vector<FrameTimerData>& ) ) );
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
        emit entitiesUpdated(mainApp->m_engine->getEntityManager()->getEntities());
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

    void Gui::MainWindow::updateFramestats( const std::vector<FrameTimerData>& stats )
    {
        QString framesA2B = QString("Frames #%1 to #%2 stats :")
                .arg( stats.front().numFrame ).arg( stats.back().numFrame );
        m_frameA2BLabel->setText(framesA2B);

        long sumRender = 0;
        long sumTasks = 0;
        long sumFrame = 0;
        long sumInterFrame = 0;

        for ( uint i = 0; i < stats.size(); ++i )
        {
            sumRender += Core::Timer::getIntervalMicro(
                        stats[i].renderData.renderStart, stats[i].renderData.renderEnd );
            sumTasks  += Core::Timer::getIntervalMicro(
                        stats[i].tasksStart, stats[i].tasksEnd );
            sumFrame  += Core::Timer::getIntervalMicro(
                        stats[i].frameStart, stats[i].frameEnd );

            if ( i > 0 )
            {
                sumInterFrame += Core::Timer::getIntervalMicro(
                            stats[i - 1].frameStart, stats[i].frameEnd );
            }
        }

        const uint N = stats.size();
        const Scalar T(N * 1000000.0);

        m_renderTime->setValue( sumRender / N );
        m_renderUpdates->setValue( T / Scalar( sumRender ) );
        m_tasksTime->setValue( sumTasks / N );
        m_tasksUpdates->setValue( T / Scalar( sumTasks ) );
        m_frameTime->setValue( sumFrame / N );
        m_frameUpdates->setValue( T / Scalar( sumFrame ) );
        m_avgFramerate->setValue( ( N - 1 ) * Scalar( 1000000.0 / sumInterFrame ) );
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

    void Gui::MainWindow::handlePicking(int drawableIndex)
    {
        if (drawableIndex >= 0)
        {
            const std::shared_ptr<Engine::RenderObject>& ro =
                mainApp->m_engine->getRenderObjectManager()->getRenderObject(drawableIndex);
            Engine::Entity* ent = ro->getComponent()->getEntity();

            int compIdx = -1;
            int i = 0;
            for (auto comp : ent->getComponentsMap())
            {
                if (comp.second == ro->getComponent())
                {
                    CORE_ASSERT(comp.first == ro->getComponent()->getName(), "Inconsistent names");
                    compIdx = i;
                    break;
                }
                ++i;
            }
            CORE_ASSERT(compIdx >= 0, "Component is not in entity");
            int entIdx = ent->idx;
            QModelIndex entityIdx = m_entityTreeModel->index(entIdx, 0);
            QModelIndex treeIdx = entityIdx.child(compIdx, 0);
            m_entitiesTreeView->setExpanded(entityIdx,true);
            m_entitiesTreeView->selectionModel()->select(treeIdx, QItemSelectionModel::SelectCurrent);
        }
        else
        {
            m_entitiesTreeView->selectionModel()->clear();
        }


    }
} // namespace Ra
