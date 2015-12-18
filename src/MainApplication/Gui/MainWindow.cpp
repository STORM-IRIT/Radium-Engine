#include <MainApplication/Gui/MainWindow.hpp>

#include <QFileDialog>
#include <QToolButton>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>
#include <MainApplication/Gui/EntityTreeItem.hpp>
#include <MainApplication/Gui/MaterialEditor.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>

#include <Plugins/Animation/AnimationSystem.hpp>
#include <assimp/Importer.hpp>

namespace Ra
{

    Gui::MainWindow::MainWindow( QWidget* parent )
        : QMainWindow( parent )
    {
        // Note : at this point most of the components (including the Engine) are
        // not initialized. Listen to the "started" signal.

        setupUi( this );

        setWindowIcon( QPixmap( ":/Assets/Images/RadiumIcon.png" ) );
        setWindowTitle( QString( "Radium Engine" ) );

        QStringList headers;
        headers << tr( "Entities -> Components" );
        m_entityTreeModel = new EntityTreeModel( headers );
        m_entitiesTreeView->setModel( m_entityTreeModel );

        m_materialEditor = new MaterialEditor();

        createConnections();

        mainApp->framesCountForStatsChanged(
            m_avgFramesCount->value() );
        m_viewer->getCameraInterface()->resetCamera();
    }

    Gui::MainWindow::~MainWindow()
    {
        // Child QObjects will automatically be deleted
    }

    void Gui::MainWindow::createConnections()
    {
        connect( actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile );
        connect( actionReload_Shaders, &QAction::triggered, m_viewer, &Viewer::reloadShaders );

        // Toolbox setup
        connect( actionToggle_Local_Global, &QAction::toggled, m_viewer->getGizmoManager(), &GizmoManager::setLocal );
        connect( actionGizmoOff,            &QAction::triggered, this, &MainWindow::gizmoShowNone );
        connect( actionGizmoTranslate,      &QAction::triggered, this, &MainWindow::gizmoShowTranslate );
        connect( actionGizmoRotate,         &QAction::triggered, this, &MainWindow::gizmoShowRotate );
        //connect( actionGizmoOff, &QAction::toggled, this, &gizmoShowNone);

        // Loading setup.
        connect( this, &MainWindow::fileLoading, mainApp, &MainApplication::loadFile );
        connect( this, &MainWindow::entitiesUpdated, m_entityTreeModel, &EntityTreeModel::entitiesUpdated );

        /* connect( m_entityTreeModel, SIGNAL( objectNameChanged( QString ) ),
                  this, SLOT( objectNameChanged( QString ) ) );*/
        // Side menu setup.
        connect( m_entityTreeModel, &EntityTreeModel::dataChanged, m_entityTreeModel, &EntityTreeModel::handleRename );

        connect( m_entitiesTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged );

        // Camera panel setup
        connect( m_cameraResetButton, &QPushButton::released, m_viewer->getCameraInterface(), &CameraInterface::resetCamera);
        connect( m_setCameraPositionButton, &QPushButton::released, this, &MainWindow::setCameraPosition);
        connect( m_setCameraTargetButton, &QPushButton::released, this, &MainWindow::setCameraTarget);

        connect( this, &MainWindow::cameraPositionSet, m_viewer->getCameraInterface(), &CameraInterface::setCameraPosition );
        connect( this, &MainWindow::cameraTargetSet,   m_viewer->getCameraInterface(), &CameraInterface::setCameraTarget );

        connect( m_viewer->getCameraInterface(), &CameraInterface::cameraPositionChanged, this, &MainWindow::onCameraPositionChanged );
        connect( m_viewer->getCameraInterface(), &CameraInterface::cameraTargetChanged, this, &MainWindow::onCameraTargetChanged );
        // Oh C++ why are you so mean to me ?
        connect( m_cameraSensitivity, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged),
            m_viewer->getCameraInterface(), &CameraInterface::setCameraSensitivity );

        // Connect picking results (TODO Val : use events to dispatch picking directly)
        connect( m_viewer, &Viewer::rightClickPicking, this, &MainWindow::handlePicking );
        connect (m_viewer, &Viewer::leftClickPicking, m_viewer->getGizmoManager(), &GizmoManager::handlePickingResult );

        // Update entities when the engine starts.
        connect( mainApp, &MainApplication::starting, this, &MainWindow::onEntitiesUpdated );

        connect( m_avgFramesCount, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            mainApp , &MainApplication::framesCountForStatsChanged );
        connect(mainApp, &MainApplication::updateFrameStats, this, &MainWindow::onUpdateFramestats);

        // Inform property editors of new selections
        connect(this, &MainWindow::selectedEntity, tab_edition, &TransformEditorWidget::setEditable);
        connect(this, &MainWindow::selectedEntity, m_viewer->getGizmoManager(), &GizmoManager::setEditable);
        connect(this, &MainWindow::selectedComponent, m_viewer->getGizmoManager(), &GizmoManager::setEditable);

        connect( this, &MainWindow::selectedEntity, this, &MainWindow::displayEntityRenderObjects );
        connect( this, &MainWindow::selectedComponent, this, &MainWindow::displayComponentRenderObjects );

        // Editors should be updated after each frame
        connect(mainApp, &MainApplication::endFrame, tab_edition, &TransformEditorWidget::updateValues);
        connect(mainApp, &MainApplication::endFrame, m_viewer->getGizmoManager(), &GizmoManager::updateValues);

        connect(playButton,  &QPushButton::clicked, this, &MainWindow::playAnimation );
        connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pauseAnimation );
        connect(stepButton,  &QPushButton::clicked, this, &MainWindow::stepAnimation );
        connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetAnimation );

        // Enable changing shaders
        connect( m_renderObjectsListView, &QListWidget::currentRowChanged, this, &MainWindow::renderObjectListItemClicked );
        connect( m_currentShaderBox, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                 this, &MainWindow::changeRenderObjectShader );

        // RO Stuff
        connect( m_toggleRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleRO );
        connect( m_removeRenderObjectButton, &QPushButton::clicked, this, &MainWindow::removeRO );
        connect( m_editRenderObjectButton, &QPushButton::clicked, this, &MainWindow::editRO );
    }

    void Gui::MainWindow::playAnimation()
    {
        AnimationPlugin::AnimationSystem* animSys = (AnimationPlugin::AnimationSystem*) mainApp->m_engine->getSystem("AnimationSystem");
        animSys->setPlaying(true);
    }

    void Gui::MainWindow::pauseAnimation()
    {
        AnimationPlugin::AnimationSystem* animSys = (AnimationPlugin::AnimationSystem*) mainApp->m_engine->getSystem("AnimationSystem");
        animSys->setPlaying(false);
    }

    void Gui::MainWindow::resetAnimation()
    {
        AnimationPlugin::AnimationSystem* animSys = (AnimationPlugin::AnimationSystem*) mainApp->m_engine->getSystem("AnimationSystem");
        animSys->reset();
    }

    void Gui::MainWindow::stepAnimation()
    {
        pauseAnimation();
        AnimationPlugin::AnimationSystem* animSys = (AnimationPlugin::AnimationSystem*) mainApp->m_engine->getSystem("AnimationSystem");
        animSys->step();
    }

    void Gui::MainWindow::onEntitiesUpdated()
    {
        emit entitiesUpdated( mainApp->m_engine->getEntityManager()->getEntities() );
    }

    void Gui::MainWindow::onCameraPositionChanged( const Core::Vector3& p )
    {
        // TODO : use a vectorEditor.
        m_cameraPositionX->setValue( p.x() );
        m_cameraPositionY->setValue( p.y() );
        m_cameraPositionZ->setValue( p.z() );
    }

    void Gui::MainWindow::onCameraTargetChanged( const Core::Vector3& p )
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
        emit cameraPositionSet( P );
    }

    void Gui::MainWindow::setCameraTarget()
    {
        Core::Vector3 T( m_cameraTargetX->value(),
                         m_cameraTargetY->value(),
                         m_cameraTargetZ->value() );
        emit cameraTargetSet( T );
    }

    void Gui::MainWindow::loadFile()
    {
        // Filter the files
        aiString extList;
        Assimp::Importer importer;
        importer.GetExtensionList(extList);
        std::string extListStd(extList.C_Str());
        std::replace(extListStd.begin(), extListStd.end(), ';', ' ');
        QString filter = QString::fromStdString(extListStd);

        QString path = QFileDialog::getOpenFileName( this, "Open File", "..", filter);
        if ( path.size() > 0 )
        {
            emit fileLoading( path );
        }

        onEntitiesUpdated();
    }

    void Gui::MainWindow::onUpdateFramestats( const std::vector<FrameTimerData>& stats )
    {
        QString framesA2B = QString( "Frames #%1 to #%2 stats :" )
                            .arg( stats.front().numFrame ).arg( stats.back().numFrame );
        m_frameA2BLabel->setText( framesA2B );

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
                                     stats[i - 1].frameEnd, stats[i].frameEnd );
            }
        }

        const uint N = stats.size();
        const Scalar T( N * 1000000.0 );

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

    void Gui::MainWindow::handlePicking( int drawableIndex )
    {
        if ( drawableIndex >= 0 )
        {
            const std::shared_ptr<Engine::RenderObject>& ro =
                mainApp->m_engine->getRenderObjectManager()->getRenderObject( drawableIndex );

            // Ignore UI render objects.
            if(ro->getType() == Engine::RenderObject::Type::RO_UI)
            {
                return;
            }

            /////////////////////////////////////////////
            LOG( logDEBUG ) << "Picked RO: " << ro->idx.getValue();
            LOG( logDEBUG ) << "RO Name  : " << ro->getName();
            /////////////////////////////////////////////

            const Engine::Component* comp = ro->getComponent();
            const Engine::Entity* ent = comp->getEntity();
            int compIdx = -1;
            int i = 0;
            for ( auto c : ent->getComponentsMap() )
            {
                if ( c.second == comp )
                {
                    CORE_ASSERT( c.first == comp->getName(), "Inconsistent names" );
                    compIdx = i;
                    break;
                }
                ++i;
            }
            CORE_ASSERT( compIdx >= 0, "Component is not in entity" );
            Core::Index entIdx = ent->idx;
            QModelIndex entityIdx = m_entityTreeModel->index( entIdx, 0 );
            QModelIndex treeIdx = entityIdx;
            if ( comp->picked(drawableIndex)) // select component.
            {
                treeIdx = entityIdx.child(compIdx, 0);
            }
            m_entitiesTreeView->setExpanded( entityIdx, true );
            m_entitiesTreeView->selectionModel()->select( treeIdx, QItemSelectionModel::SelectCurrent );

            auto foundItems = m_renderObjectsListView->findItems( QString( ro->getName().c_str() ), Qt::MatchExactly );
            if  ( foundItems.size() > 0 )
            {
                m_renderObjectsListView->setCurrentItem( foundItems.at( 0 ) );
            }
            else
            {
                LOG( logERROR ) << "Not found";
            }
        }
        else
        {
            m_entitiesTreeView->selectionModel()->clear();
        }


    }

    void Gui::MainWindow::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
    {
        if ( selected.size() > 0 )
        {
            QModelIndex selIdx = selected.indexes()[0];

            Engine::Entity* entity = m_entityTreeModel->getItem(selIdx)->getData(0).entity;
            if (entity)
            {
                // Debug entity and objects are not selectable
                if (entity != Engine::SystemEntity::getInstance())
                {
                    emit selectedEntity(entity);
                }
            }
            else
            {
                Engine::Component* comp = m_entityTreeModel->getItem(selIdx)->getData(0).component;
                emit selectedComponent(comp);
            }
        }
        else
        {
            emit selectedEntity( nullptr );
            emit selectedComponent( nullptr );
        }
    }

    void Gui::MainWindow::closeEvent( QCloseEvent *event )
    {
        emit closed();
        event->accept();
    }

    void Gui::MainWindow::gizmoShowNone()
    {
        m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::NONE);
    }

    void Gui::MainWindow::gizmoShowTranslate()
    {
        m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::TRANSLATION);
    }

    void Gui::MainWindow::gizmoShowRotate()
    {
        m_viewer->getGizmoManager()->changeGizmoType(GizmoManager::ROTATION);
    }

    void Gui::MainWindow::displayEntityRenderObjects( Engine::Entity* entity )
    {
        m_renderObjectsListView->clear();
        m_currentShaderBox->setCurrentText( "" );

        // NOTE(Charly): When clicking on UI stuff, the returned entity is null
        if ( nullptr == entity )
        {
            m_selectedStuffName->clear();
            return;
        }

        m_renderObjectsListView->clear();

        QString text( "Entity : " );
        text.append( entity->getName().c_str() );
        m_selectedStuffName->setText( text );

        auto comps = entity->getComponentsMap();

        for ( const auto comp : comps )
        {
            displayRenderObjects( comp.second );
        }
    }

    void Gui::MainWindow::displayComponentRenderObjects( Engine::Component* component )
    {
        // NOTE(Charly): When clicking on UI stuff, or on nothing, the returned component is null
        m_renderObjectsListView->clear();
        m_currentShaderBox->setCurrentText( "" );

        if ( nullptr == component )
        {
            m_selectedStuffName->clear();
            return;
        }

        QString text( "Entity : " );
        text.append( component->getName().c_str() );
        m_selectedStuffName->setText( text );

        displayRenderObjects( component );
    }

    void Gui::MainWindow::displayRenderObjects( Engine::Component* component )
    {
        auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        for ( Core::Index idx : component->renderObjects )
        {
            QString name = roMgr->getRenderObject( idx )->getName().c_str();

            QListWidgetItem* item = new QListWidgetItem( name, m_renderObjectsListView );
            item->setData( 1, QVariant( idx.getValue() ) );
        }
    }

    void Gui::MainWindow::renderObjectListItemClicked( int idx )
    {
        if ( idx < 0 )
        {
            // Got out of scope
            return;
        }

        QListWidgetItem* item = m_renderObjectsListView->item( idx );
        Core::Index itemIdx( item->data( 1 ).toInt() );

        auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        auto ro = roMgr->getRenderObject( itemIdx );

        auto shaderName = ro->getRenderTechnique()->shader->getBasicConfiguration().getName();

        if ( m_currentShaderBox->findText( shaderName.c_str() ) == -1 )
        {
            m_currentShaderBox->setCurrentText( "" );
        }
        else
        {
            m_currentShaderBox->setCurrentText( shaderName.c_str() );
        }
    }

    void Gui::MainWindow::changeRenderObjectShader( const QString& shaderName )
    {
        if ( shaderName == "" )
        {
            return;
        }

        auto ro = getSelectedRO();
        if ( ro == nullptr )
        {
            return;
        }

        if ( ro->getRenderTechnique()->shader->getBasicConfiguration().getName() == shaderName.toStdString() )
        {
            return;
        }

        Engine::ShaderConfiguration config;

        config.setName( shaderName.toStdString() );
        config.setPath( "../Shaders" );

        if ( shaderName == "BlinnPhong" || shaderName == "Plain" )
        {
            config.setType( Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM );
        }
        if ( shaderName == "BlinnPhongWireframe" || shaderName == "Lines" )
        {
            config.setType( Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM );
        }

        ro->getRenderTechnique()->changeShader( config );
    }

    void Gui::MainWindow::toggleRO()
    {
        auto ro = getSelectedRO();

        if ( ro == nullptr )
        {
            return;
        }

        ro->setVisible( !ro->isVisible() );
    }

    void Gui::MainWindow::removeRO()
    {
        LOG( logINFO ) << "Not implemented yet";
    }

    void Gui::MainWindow::editRO()
    {
        m_materialEditor->changeRenderObject( getSelectedRO()->idx );
        m_materialEditor->show();
    }

    std::shared_ptr<Engine::RenderObject> Gui::MainWindow::getSelectedRO()
    {
        QListWidgetItem* item = m_renderObjectsListView->currentItem();

        if ( nullptr == item )
        {
            return nullptr;
        }

        Core::Index itemIdx( item->data( 1 ).toInt() );

        auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
        auto ro = roMgr->getRenderObject( itemIdx );

        return ro;
    }

} // namespace Ra
