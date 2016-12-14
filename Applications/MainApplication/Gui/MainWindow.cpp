#include <Gui/MainWindow.hpp>

#include <QSettings>
#include <QFileDialog>
#include <QToolButton>

#include <assimp/Importer.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <GuiBase/Viewer/CameraInterface.hpp>
#include <GuiBase/TreeModel/EntityTreeModel.hpp>

#include <Gui/MaterialEditor.hpp>
#include <Gui/MainWindow.hpp>

#include <MainApplication.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>

using Ra::Engine::ItemEntry;

namespace Ra
{

    Gui::MainWindow::MainWindow(QWidget* parent)
            : QMainWindow(parent)
    {
        // Note : at this point most of the components (including the Engine) are
        // not initialized. Listen to the "started" signal.

        setupUi(this);

        /*m_viewer = new Ra::Gui::Viewer(m_centralWidget);
        m_viewer->setObjectName(QStringLiteral("m_viewer"));
        m_viewer->setEnabled(true);
        m_viewer->setMinimumSize(QSize(800, 600));

        gridLayout_2->addWidget(m_viewer, 0, 0, 1, 1);*/

        setWindowIcon(QPixmap(":/Assets/Images/RadiumIcon.png"));
        setWindowTitle(QString("Radium Engine"));

        QStringList headers;
        headers << tr("Entities -> Components");
        m_itemModel = new GuiBase::ItemModel(mainApp->getEngine(), this);
        m_entitiesTreeView->setModel(m_itemModel);
        m_materialEditor = new MaterialEditor();
        m_selectionManager = new GuiBase::SelectionManager(m_itemModel, this);
        m_entitiesTreeView->setSelectionModel(m_selectionManager);

        createConnections();

        mainApp->framesCountForStatsChanged((uint) m_avgFramesCount->value());
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
        connect(actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile);
        connect(actionReload_Shaders, &QAction::triggered, m_viewer, &Viewer::reloadShaders);
        connect(actionOpen_Material_Editor, &QAction::triggered, this, &MainWindow::openMaterialEditor);

        // Toolbox setup
        connect(actionToggle_Local_Global, &QAction::toggled, m_viewer->getGizmoManager(), &GizmoManager::setLocal);
        connect(actionGizmoOff, &QAction::triggered, this, &MainWindow::gizmoShowNone);
        connect(actionGizmoTranslate, &QAction::triggered, this, &MainWindow::gizmoShowTranslate);
        connect(actionGizmoRotate, &QAction::triggered, this, &MainWindow::gizmoShowRotate);

        connect(actionRecord_Frames, &QAction::toggled, mainApp, &MainApplication::setRecordFrames);

        // Loading setup.
        connect(this, &MainWindow::fileLoading, mainApp, &BaseApplication::loadFile);

        // Connect picking results (TODO Val : use events to dispatch picking directly)
        connect(m_viewer, &Viewer::rightClickPicking, this, &MainWindow::handlePicking);
        connect(m_viewer, &Viewer::leftClickPicking, m_viewer->getGizmoManager(), &GizmoManager::handlePickingResult);

        connect(m_avgFramesCount, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                mainApp, &BaseApplication::framesCountForStatsChanged);
        connect(mainApp, &BaseApplication::updateFrameStats, this, &MainWindow::onUpdateFramestats);

        // Inform property editors of new selections
        connect(m_selectionManager, &GuiBase::SelectionManager::selectionChanged, this, &MainWindow::onSelectionChanged);
        //connect(this, &MainWindow::selectedItem, tab_edition, &TransformEditorWidget::setEditable);

        // Make selected item event visible to plugins
        connect(this, &MainWindow::selectedItem, mainApp, &MainApplication::onSelectedItem);
        connect(this, &MainWindow::selectedItem, m_viewer->getGizmoManager(), &GizmoManager::setEditable);
        connect(this, &MainWindow::selectedItem, m_viewer->getGizmoManager(), &GizmoManager::setEditable);

        // Enable changing shaders
        connect(m_currentShaderBox, static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                this, &MainWindow::changeRenderObjectShader);

        // RO Stuff
        connect(m_toggleRenderObjectButton, &QPushButton::clicked, this, &MainWindow::toggleVisisbleRO);
        connect(m_editRenderObjectButton, &QPushButton::clicked, this, &MainWindow::editRO);

        // Renderer stuff
        connect(m_viewer, &Viewer::rendererReady, this, &MainWindow::onRendererReady);

        connect(m_displayedTextureCombo,
                static_cast<void (QComboBox::*)(const QString&)>( &QComboBox::currentIndexChanged ),
                m_viewer, &Viewer::displayTexture);

        connect(m_enablePostProcess, &QCheckBox::stateChanged, m_viewer, &Viewer::enablePostProcess);
        connect(m_enableDebugDraw,   &QCheckBox::stateChanged, m_viewer, &Viewer::enableDebugDraw);
        connect(m_realFrameRate,     &QCheckBox::stateChanged, mainApp,  &BaseApplication::setRealFrameRate);

        // Connect engine signals to the appropriate callbacks
        std::function<void(const Engine::ItemEntry&)> add = std::bind(&MainWindow::onItemAdded, this, std::placeholders::_1);
        std::function<void(const Engine::ItemEntry&)> del = std::bind(&MainWindow::onItemRemoved, this, std::placeholders::_1);
        mainApp->m_engine->getSignalManager()->m_entityCreatedCallbacks.push_back(add);
        mainApp->m_engine->getSignalManager()->m_entityDestroyedCallbacks.push_back(del);

        mainApp->m_engine->getSignalManager()->m_componentAddedCallbacks.push_back(add);
        mainApp->m_engine->getSignalManager()->m_componentRemovedCallbacks.push_back(del);

        mainApp->m_engine->getSignalManager()->m_roAddedCallbacks.push_back(add);
        mainApp->m_engine->getSignalManager()->m_roRemovedCallbacks.push_back(del);

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

        QSettings settings;
        QString path = settings.value("files/load", QDir::homePath()).toString();
        path = QFileDialog::getOpenFileName(this, "Open File", path, filter);
        if (path.size() > 0)
        {
            settings.setValue("files/load", path);
            emit fileLoading(path);
        }
    }

    void Gui::MainWindow::onUpdateFramestats(const std::vector<FrameTimerData>& stats)
    {
        QString framesA2B = QString("Frames #%1 to #%2 stats :")
                .arg(stats.front().numFrame).arg(stats.back().numFrame);
        m_frameA2BLabel->setText(framesA2B);

        long sumEvents = 0;
        long sumRender = 0;
        long sumTasks = 0;
        long sumFrame = 0;
        long sumInterFrame = 0;

        for (uint i = 0; i < stats.size(); ++i)
        {
            sumEvents += Core::Timer::getIntervalMicro(stats[i].eventsStart, stats[i].eventsEnd);
            sumRender += Core::Timer::getIntervalMicro(stats[i].renderData.renderStart, stats[i].renderData.renderEnd);
            sumTasks += Core::Timer::getIntervalMicro(stats[i].tasksStart, stats[i].tasksEnd);
            sumFrame += Core::Timer::getIntervalMicro(stats[i].frameStart, stats[i].frameEnd);

            if (i > 0)
            {
                sumInterFrame += Core::Timer::getIntervalMicro(stats[i - 1].frameEnd, stats[i].frameEnd);
            }
        }

        const uint N(stats.size());
        const Scalar T(N * 1000000.f);

        m_eventsTime->setValue(sumEvents / N);
        m_eventsUpdates->setValue(T / Scalar(sumEvents));
        m_renderTime->setValue(sumRender / N);
        m_renderUpdates->setValue(T / Scalar(sumRender));
        m_tasksTime->setValue(sumTasks / N);
        m_tasksUpdates->setValue(T / Scalar(sumTasks));
        m_frameTime->setValue(sumFrame / N);
        m_frameUpdates->setValue(T / Scalar(sumFrame));
        m_avgFramerate->setValue((N - 1) * Scalar(1000000.0 / sumInterFrame));
    }

    Gui::Viewer* Gui::MainWindow::getViewer()
    {
        return m_viewer;
    }

    GuiBase::SelectionManager* Gui::MainWindow::getSelectionManager()
    {
        return m_selectionManager;
    }

    void Gui::MainWindow::handlePicking(int pickingResult)
    {
        Ra::Core::Index roIndex(pickingResult);
        Ra::Engine::RadiumEngine* engine = Ra::Engine::RadiumEngine::getInstance();
        if (roIndex.isValid())
        {
            Ra::Engine::Component* comp = engine->getRenderObjectManager()->getRenderObject(roIndex)->getComponent();
            Ra::Engine::Entity* ent = comp->getEntity();

            // For now we don't enable group selection.
            m_selectionManager->setCurrentEntry(ItemEntry(ent, comp, roIndex), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        }
        else
        {
            m_selectionManager->clear();
        }
    }

    void Gui::MainWindow::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        if (m_selectionManager->hasSelection())
        {
            const ItemEntry& ent = m_selectionManager->currentItem();
            emit selectedItem(ent);
            m_selectedItemName->setText(QString::fromStdString(getEntryName(mainApp->getEngine(), ent)));
            m_editRenderObjectButton->setEnabled(false);

            if (ent.isRoNode())
            {
                m_editRenderObjectButton->setEnabled(true);

                m_materialEditor->changeRenderObject(ent.m_roIndex);
                const std::string& shaderName = mainApp->m_engine->getRenderObjectManager()->getRenderObject(
                                                               ent.m_roIndex)
                                                       ->getRenderTechnique()->shader->getBasicConfiguration().m_name;


                if (m_currentShaderBox->findText(shaderName.c_str()) == -1)
                {
                    m_currentShaderBox->addItem(QString(shaderName.c_str()));
                    m_currentShaderBox->setCurrentText(shaderName.c_str());
                }
                else
                {
                    m_currentShaderBox->setCurrentText(shaderName.c_str());
                }
            }
        }
        else
        {
            emit selectedItem(ItemEntry());
            m_selectedItemName->setText("");
            m_editRenderObjectButton->setEnabled(false);
            m_materialEditor->hide();
        }
    }

    void Gui::MainWindow::closeEvent(QCloseEvent* event)
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


    void Gui::MainWindow::changeRenderObjectShader(const QString& shaderName)
    {
        std::string name = shaderName.toStdString();
        if (name == "")
        {
            return;
        }

        const ItemEntry& item = m_selectionManager->currentItem();
        if (!item.isRoNode())
        {
            return;
        }

        const auto& ro = mainApp->m_engine->getRenderObjectManager()->getRenderObject(item.m_roIndex);
        if (ro->getRenderTechnique()->shader->getBasicConfiguration().m_name == name)
        {
            return;
        }
        Engine::ShaderConfiguration config = Ra::Engine::ShaderConfigurationFactory::getConfiguration(name);
        ro->getRenderTechnique()->changeShader(config);
    }

    void Gui::MainWindow::toggleVisisbleRO()
    {
        const ItemEntry& item = m_selectionManager->currentItem();
        // If at least one RO is visible, turn them off.
        bool hasVisible = false;
        for (auto roIdx : getItemROs(mainApp->m_engine.get(), item))
        {
            if (mainApp->m_engine->getRenderObjectManager()->getRenderObject(roIdx)->isVisible())
            {
                hasVisible = true;
                break;
            }
        }
        for (auto roIdx : getItemROs(mainApp->m_engine.get(), item))
        {
            mainApp->m_engine->getRenderObjectManager()->getRenderObject(roIdx)->setVisible(!hasVisible);
        }
    }

    void Gui::MainWindow::editRO()
    {
        ItemEntry item = m_selectionManager->currentItem();
        if (item.isRoNode())
        {
            m_materialEditor->changeRenderObject(item.m_roIndex);
            m_materialEditor->show();
        }
    }

    void Gui::MainWindow::openMaterialEditor()
    {
        m_materialEditor->show();
    }

    void Gui::MainWindow::updateUi(Plugins::RadiumPluginInterface* plugin)
    {
        QString tabName;

        if (plugin->doAddMenu())
        {
            QMainWindow::menuBar()->addMenu(plugin->getMenu());
            // Add menu
        }

        if (plugin->doAddWidget(tabName))
        {
            toolBox->addItem(plugin->getWidget(), tabName);
        }
    }

    void Gui::MainWindow::onRendererReady()
    {
        m_viewer->getCameraInterface()->resetCamera();

        QSignalBlocker blockTextures(m_displayedTextureCombo);

        auto texs = m_viewer->getRenderer()->getAvailableTextures();
        for (const auto& tex : texs)
        {
            m_displayedTextureCombo->addItem(tex.c_str());
        }
    }

    void Gui::MainWindow::onFrameComplete()
    {
        tab_edition->updateValues();
        m_viewer->getGizmoManager()->updateValues();
    }

    void Gui::MainWindow::onItemAdded(const Engine::ItemEntry& ent)
    {
        m_itemModel->addItem(ent);
    }

    void Gui::MainWindow::onItemRemoved(const Engine::ItemEntry& ent)
    {
        m_itemModel->removeItem(ent) ;
    }



} // namespace Ra
