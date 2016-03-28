#include <MainApplication/Gui/MainWindow.hpp>

#include <QFileDialog>
#include <QToolButton>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>
#include <MainApplication/Gui/EntityTreeItem.hpp>
#include <MainApplication/Gui/MaterialEditor.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>

#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>
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

        m_lastSelectedRO = -1;
        createConnections();

        mainApp->framesCountForStatsChanged( m_avgFramesCount->value() );
    }

    Gui::MainWindow::~MainWindow()
    {
        // Child QObjects will automatically be deleted
    }


    void Gui::MainWindow::cleanup()
    {
        gizmoShowNone();
    }

    void Gui::MainWindow::createConnections()
    {
        connect( actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile );
        connect( actionReload_Shaders, &QAction::triggered, m_viewer, &Viewer::reloadShaders );
        connect( actionOpen_Material_Editor, &QAction::triggered, this, &MainWindow::openMaterialEditor );

        // Toolbox setup
        connect( actionToggle_Local_Global, &QAction::toggled, m_viewer->getGizmoManager(), &GizmoManager::setLocal );
        connect( actionGizmoOff,            &QAction::triggered, this, &MainWindow::gizmoShowNone );
        connect( actionGizmoTranslate,      &QAction::triggered, this, &MainWindow::gizmoShowTranslate );
        connect( actionGizmoRotate,         &QAction::triggered, this, &MainWindow::gizmoShowRotate );

        // Loading setup.
        connect( this, &MainWindow::fileLoading, mainApp, &MainApplication::loadFile );
        connect( mainApp, &MainApplication::loadComplete, this, &MainWindow::onEntitiesUpdated);

        // Side menu setup.
        connect( m_entityTreeModel, &EntityTreeModel::dataChanged, m_entityTreeModel, &EntityTreeModel::handleRename );

        connect( m_entitiesTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged );

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

        // Enable changing shaders
        connect( m_renderObjectsListView, &QListWidget::currentRowChanged, this, &MainWindow::renderObjectListItemClicked );
        connect( m_currentShaderBox, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                 this, &MainWindow::changeRenderObjectShader );

        // RO Stuff
        connect( m_toggleRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleVisisbleRO );
        connect( m_removeRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleXRayRO );
        connect( m_editRenderObjectButton, &QPushButton::clicked, this, &MainWindow::editRO );

        // Renderer stuff
        connect( m_viewer, &Viewer::rendererReady, this, &MainWindow::onRendererReady );

        connect( m_displayedTextureCombo, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                 m_viewer, &Viewer::displayTexture );
        connect( m_currentRendererCombo, static_cast<void (QComboBox::*)( int )>( &QComboBox::currentIndexChanged ),
                 m_viewer, &Viewer::changeRenderer );

        connect(m_enablePostProcess, &QCheckBox::stateChanged, m_viewer, &Viewer::enablePostProcess);

        // Connect engine signals to the appropriate callbacks
        std::function<void(void)> f =  std::bind(&MainWindow::onEntitiesUpdated, this);
        mainApp->m_engine->getSignalManager()->m_entityCreatedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_entityDestroyedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_componentAddedCallbacks.push_back(f);
        mainApp->m_engine->getSignalManager()->m_componentRemovedCallbacks.push_back(f);

    }

    void Gui::MainWindow::onEntitiesUpdated()
    {
        m_entityTreeModel->entitiesUpdated();
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
    }

    void Gui::MainWindow::onUpdateFramestats( const std::vector<FrameTimerData>& stats )
    {
        QString framesA2B = QString( "Frames #%1 to #%2 stats :" )
                            .arg( stats.front().numFrame ).arg( stats.back().numFrame );
        m_frameA2BLabel->setText( framesA2B );

        long sumEvents = 0;
        long sumRender = 0;
        long sumTasks = 0;
        long sumFrame = 0;
        long sumInterFrame = 0;

        for ( uint i = 0; i < stats.size(); ++i )
        {
            sumEvents += Core::Timer::getIntervalMicro(stats[i].eventsStart, stats[i].eventsEnd);
            sumRender += Core::Timer::getIntervalMicro(stats[i].renderData.renderStart, stats[i].renderData.renderEnd );
            sumTasks  += Core::Timer::getIntervalMicro(stats[i].tasksStart, stats[i].tasksEnd );
            sumFrame  += Core::Timer::getIntervalMicro(stats[i].frameStart, stats[i].frameEnd );

            if ( i > 0 )
            {
                sumInterFrame += Core::Timer::getIntervalMicro(stats[i - 1].frameEnd, stats[i].frameEnd );
            }
        }

        const uint N = stats.size();
        const Scalar T( N * 1000000.0 );

        m_eventsTime->setValue(sumEvents / N);
        m_eventsUpdates->setValue(T / Scalar(sumEvents));
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
        if (m_lastSelectedRO >= 0)
        {

            if ( mainApp->m_engine->getRenderObjectManager()->exists( m_lastSelectedRO ))
            {
                const std::shared_ptr<Engine::RenderObject>& ro =
                    mainApp->m_engine->getRenderObjectManager()->getRenderObject( m_lastSelectedRO );

                Engine::Component* comp = ro->getComponent();
                Engine::Entity* ent = comp->getEntity();
                comp->picked( -1 );
                ent->picked(-1);
            }
        }

        if ( drawableIndex >= 0 )
        {
            const std::shared_ptr<Engine::RenderObject>& ro =
                mainApp->m_engine->getRenderObjectManager()->getRenderObject( drawableIndex );

            // Ignore UI render objects.
            if(ro->getType() == Engine::RenderObjectType::UI)
            {
                return;
            }

            /////////////////////////////////////////////
            LOG( logDEBUG ) << "Picked RO: " << ro->idx.getValue();
            LOG( logDEBUG ) << "RO Name  : " << ro->getName();
            /////////////////////////////////////////////

            Engine::Component* comp = ro->getComponent();
            const Engine::Entity* ent = comp->getEntity();
            int compIdx = -1;
            int i = 0;
            for ( const auto& c : ent->getComponents() )
            {
                if ( c.get() == comp )
                {
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
        m_lastSelectedRO = drawableIndex;
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


        for ( const auto& comp : entity->getComponents())
        {
            displayRenderObjects( comp.get() );
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
        for ( Core::Index idx : component->m_renderObjects )
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

        auto shaderName = ro->getRenderTechnique()->shader->getBasicConfiguration().m_name;

        m_materialEditor->changeRenderObject( ro->idx );

        if ( m_currentShaderBox->findText( shaderName.c_str() ) == -1 )
        {
            m_currentShaderBox->addItem( QString( shaderName.c_str() ) );
            m_currentShaderBox->setCurrentText( shaderName.c_str() );
        }
        else
        {
            m_currentShaderBox->setCurrentText( shaderName.c_str() );
        }
    }

    void Gui::MainWindow::changeRenderObjectShader( const QString& shaderName )
    {
        std::string name = shaderName.toStdString();
        if ( name == "" )
        {
            return;
        }

        auto ro = getSelectedRO();
        if ( ro == nullptr )
        {
            return;
        }

        if (ro->getRenderTechnique()->shader->getBasicConfiguration().m_name == name)
        {
            return;
        }

        Engine::ShaderConfiguration config = Ra::Engine::ShaderConfigurationFactory::getConfiguration(name);
        ro->getRenderTechnique()->changeShader(config);
    }

    void Gui::MainWindow::toggleVisisbleRO()
    {
        auto ro = getSelectedRO();

        if ( ro == nullptr )
        {
            return;
        }

        ro->toggleVisible();
    }

    void Gui::MainWindow::toggleXRayRO()
    {
        auto ro = getSelectedRO();

        if ( ro == nullptr )
        {
            return;
        }

        ro->toggleXRay();
    }

    void Gui::MainWindow::editRO()
    {
        auto ro = getSelectedRO();
        m_materialEditor->changeRenderObject( ro ? ro->idx : Core::Index::INVALID_IDX() );
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

    void Gui::MainWindow::openMaterialEditor()
    {
        m_materialEditor->show();
    }

    void Gui::MainWindow::updateUi( Plugins::RadiumPluginInterface *plugin )
    {
        QString tabName;

        if ( plugin->doAddMenu() )
        {
            QMainWindow::menuBar()->addMenu( plugin->getMenu() );
            // Add menu
        }

        if ( plugin->doAddWidget( tabName ) )
        {
            toolBox->addItem( plugin->getWidget(), tabName );
        }
    }

    void Gui::MainWindow::onRendererReady()
    {
        m_viewer->getCameraInterface()->resetCamera();

        QSignalBlocker blockTextures( m_displayedTextureCombo );
        QSignalBlocker blockRenderer( m_currentRendererCombo );

        auto texs = m_viewer->getRenderer()->getAvailableTextures();
        for ( const auto& tex : texs )
        {
            m_displayedTextureCombo->addItem( tex.c_str() );
        }

        for ( const auto& renderer : m_viewer->getRenderersName() )
        {
            m_currentRendererCombo->addItem( renderer.c_str() );
        }
    }

    void Gui::MainWindow::onFrameComplete()
    {
        tab_edition->updateValues();
        m_viewer->getGizmoManager()->updateValues();
    }
} // namespace Ra
