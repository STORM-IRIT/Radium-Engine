#include <SkinningPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <SkinningSystem.hpp>

#include <QFileDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace SkinningPlugin {

SkinningPluginC::~SkinningPluginC() {}

void SkinningPluginC::registerPlugin( const Ra::PluginContext& context ) {
    m_system = new SkinningSystem;
    m_selectionManager = context.m_selectionManager;
    context.m_engine->registerSystem( "SkinningSystem", m_system );
    m_widget = new SkinningWidget;
    connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this,
             &SkinningPluginC::onCurrentChanged );
}

bool SkinningPluginC::doAddWidget( QString& name ) {
    name = "Skinning";
    return true;
}

QWidget* SkinningPluginC::getWidget() {
    return m_widget;
}

bool SkinningPluginC::doAddMenu() {
    return false;
}

QMenu* SkinningPluginC::getMenu() {
    return nullptr;
}

bool SkinningPluginC::doAddAction( int& nb ) {
    nb = 4;
    return true;
}

QAction* SkinningPluginC::getAction( int id ) {
    switch ( id )
    {
    case 0:
        return m_widget->m_actionLBS;
    case 1:
        return m_widget->m_actionDQ;
    case 2:
        return m_widget->m_actionCoR;
    case 3:
        return m_widget->m_actionPBS;
    default:
        return nullptr;
    }
}

void SkinningPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev ) {
    Ra::Engine::ItemEntry it = m_selectionManager->currentItem();
    if ( it.m_entity )
    {
        auto comps = m_system->getEntityComponents( it.m_entity );
        if ( comps.size() != 0 )
            m_widget->setCurrent( it, static_cast<SkinningPlugin::SkinningComponent*>( comps[0] ) );
        else
            m_widget->setCurrent( it, nullptr );
    } else
        m_widget->setCurrent( it, nullptr );
}

// Class SkinningWidget

SkinningWidget::SkinningWidget( QWidget* parent ) : QFrame( parent ), m_current( nullptr ) {
    m_skinningSelect = new QComboBox( this );
    m_skinningSelect->setMaxVisibleItems( 4 );
    m_skinningSelect->setMaxCount( 4 );
    m_skinningSelect->setDuplicatesEnabled( false );
    m_skinningSelect->setCurrentIndex( 1 );

    m_skinningSelect->insertItems( 0, QStringList() << "Linear Blend Skinning"
                                                    << "Dual Quaternion Skinning"
                                                    << "Center of Rotation skinning"
                                                    << "Position Based Skinning" );
    m_skinningSelect->setEnabled( false );

    m_actionLBS =
        new QAction( QIcon( ":/Assets/Images/LB.png" ), QString( "Linear Blending" ), nullptr );
    m_actionDQ =
        new QAction( QIcon( ":/Assets/Images/DQ_on.png" ), QString( "Dual Quaternion" ), nullptr );
    m_actionCoR =
        new QAction( QIcon( ":/Assets/Images/CoR.png" ), QString( "Center of Rotation" ), nullptr );
    m_actionPBS = new QAction( QIcon( ":/Assets/Images/PBS.png" ),
                               QString( "Position Based Skinning" ), nullptr );
    m_actionLBS->setEnabled( false );
    m_actionDQ->setEnabled( false );
    m_actionCoR->setEnabled( false );
    m_actionPBS->setEnabled( false );

    connect( m_skinningSelect,
             static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ), this,
             &SkinningWidget::onSkinningChanged );

    connect( m_actionLBS, &QAction::triggered, this, &SkinningWidget::onLSBActionTriggered );
    connect( m_actionDQ, &QAction::triggered, this, &SkinningWidget::onDQActionTriggered );
    connect( m_actionCoR, &QAction::triggered, this, &SkinningWidget::onCoRActionTriggered );
    connect( m_actionPBS, &QAction::triggered, this, &SkinningWidget::onPBSActionTriggered );

    setSkinningWidgetLayout();
}

void SkinningWidget::setSkinningWidgetLayout() {
    QWidget* pbsPageWidget = createPBSInterface();
    QWidget* noOptionsPageWidget = new QWidget;

    m_stackedWidget = new QStackedWidget;
    m_stackedWidget->addWidget( noOptionsPageWidget );
    m_stackedWidget->addWidget( pbsPageWidget );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_skinningSelect );
    layout->addWidget( m_stackedWidget );
    setLayout( layout );
}

QWidget* SkinningWidget::createPBSInterface() {
    QWidget* pbsPageWidget = new QWidget;
    QFormLayout* pbsLayout = new QFormLayout;

    createPBStiffnesses();

    QLabel* volumetricMeshPathLabel = new QLabel{tr( "Volumetric Mesh Path :" )};
    m_pathVolumetricMesh = new QTextEdit{};
    m_pathVolumetricMesh->setFixedHeight( 60 );
    m_pathVolumetricMesh->setReadOnly( true );
    QPushButton* volumetricMeshPathSelect = new QPushButton{"Choose Volumetric Mesh Path"};
    connect( volumetricMeshPathSelect, SIGNAL( released() ), this,
             SLOT( chooseVolumetricMeshPath() ) );
    m_pathVolumetricMeshValidIndication = new QLabel{tr( "No Volumetric Mesh File Selected" )};

    pbsLayout->addRow( tr( "&Stretch Stifness :" ), m_stretchStiffnessSelect );
    pbsLayout->addRow( tr( "&Compression Stiffness :" ), m_compressionStiffnessSelect );
    pbsLayout->addRow( tr( "&Neg Volume Stiffness :" ), m_negVolumeStiffnessSelect );
    pbsLayout->addRow( tr( "&Pos Volume Stiffness :" ), m_posVolumeStiffnessSelect );
    pbsLayout->addRow( volumetricMeshPathLabel );
    pbsLayout->addRow( volumetricMeshPathSelect );
    pbsLayout->addRow( m_pathVolumetricMesh );
    pbsLayout->addRow( m_pathVolumetricMeshValidIndication );

    pbsPageWidget->setLayout( pbsLayout );
    return pbsPageWidget;
}

void SkinningWidget::createPBStiffnesses() {
    m_stretchStiffnessSelect = new QDoubleSpinBox();
    m_compressionStiffnessSelect = new QDoubleSpinBox();
    m_negVolumeStiffnessSelect = new QDoubleSpinBox();
    m_posVolumeStiffnessSelect = new QDoubleSpinBox();

    // Step
    const double step = 0.1;
    m_stretchStiffnessSelect->setSingleStep( step );
    m_compressionStiffnessSelect->setSingleStep( step );
    m_negVolumeStiffnessSelect->setSingleStep( step );
    m_posVolumeStiffnessSelect->setSingleStep( step );

    // Range
    m_stretchStiffnessSelect->setRange( 0.0, 1.0 );
    m_compressionStiffnessSelect->setRange( 0.0, 1.0 );
    m_negVolumeStiffnessSelect->setRange( 0.0, 1.0 );
    m_posVolumeStiffnessSelect->setRange( 0.0, 1.0 );

    // Initial Value
    const double initialValue = 1.0;
    m_stretchStiffnessSelect->setValue( initialValue );
    m_compressionStiffnessSelect->setValue( initialValue );
    m_negVolumeStiffnessSelect->setValue( initialValue );
    m_posVolumeStiffnessSelect->setValue( initialValue );

    // Signal connection
    connect( m_stretchStiffnessSelect, SIGNAL( valueChanged( double ) ), this,
             SLOT( onStretchStiffnessSelectValueChanged( double ) ) );

    connect( m_compressionStiffnessSelect, SIGNAL( valueChanged( double ) ), this,
             SLOT( onCompressionSelectValueChanged( double ) ) );

    connect( m_negVolumeStiffnessSelect, SIGNAL( valueChanged( double ) ), this,
             SLOT( onNegVolumeSelectValueChanged( double ) ) );

    connect( m_posVolumeStiffnessSelect, SIGNAL( valueChanged( double ) ), this,
             SLOT( onPosVolumeStiffnessSelectValueChanged( double ) ) );
}

void SkinningWidget::onLSBActionTriggered() {
    m_skinningSelect->setCurrentIndex( 0 );
}

void SkinningWidget::onDQActionTriggered() {
    m_skinningSelect->setCurrentIndex( 1 );
}

void SkinningWidget::onCoRActionTriggered() {
    m_skinningSelect->setCurrentIndex( 2 );
}

void SkinningWidget::onPBSActionTriggered() {
    m_skinningSelect->setCurrentIndex( 3 );
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    if ( !pbs.isVolumetricMeshValid() )
    {
        chooseVolumetricMeshPath();
    }
}

void SkinningWidget::onStretchStiffnessSelectValueChanged( double value ) {
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    PBD::SimulationModel& pbs_model = pbs.getModel();
    //       pbs_model.setStretchStiffness(value);
}

void SkinningWidget::onCompressionSelectValueChanged( double value ) {
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    PBD::SimulationModel& pbs_model = pbs.getModel();
    //     pbs_model.setCompressionStiffness(value);
}

void SkinningWidget::onNegVolumeSelectValueChanged( double value ) {
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    PBD::SimulationModel& pbs_model = pbs.getModel();
    //    pbs_model.setNegVolumeStiffness(value);
}

void SkinningWidget::onPosVolumeStiffnessSelectValueChanged( double value ) {
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    PBD::SimulationModel& pbs_model = pbs.getModel();
    //    pbs_model.setPosVolumeStiffness(value);
}

void SkinningWidget::chooseVolumetricMeshPath() {
    QString fileName =
        QFileDialog::getOpenFileName( this, tr( "Choose the Corresponding Volumetric Mesh File" ),
                                      "", tr( "Mesh Files (*.mesh)" ) );
    PositionBasedSkinning::PBS& pbs = m_current->getPBS();
    pbs.setVolumetricMeshPath( fileName.toStdString() );

    // Update path
    m_pathVolumetricMesh->setText( fileName );
    m_pathVolumetricMesh->setToolTip( fileName );

    if ( !pbs.isVolumetricMeshValid() )
    {
        m_pathVolumetricMeshValidIndication->setText(
            tr( "The selected Volumetric Mesh File is not Valid" ) );
    } else
    { m_pathVolumetricMeshValidIndication->setText( tr( "" ) ); }
}

void SkinningWidget::setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp ) {
    m_current = comp;
    if ( comp )
    {
        m_skinningSelect->setEnabled( true );
        m_actionLBS->setEnabled( true );
        m_actionDQ->setEnabled( true );
        m_actionCoR->setEnabled( true );
        m_actionPBS->setEnabled( true );
        m_skinningSelect->setCurrentIndex( int( comp->getSkinningType() ) );
    } else
    {
        m_skinningSelect->setEnabled( false );
        m_actionLBS->setEnabled( false );
        m_actionDQ->setEnabled( false );
        m_actionCoR->setEnabled( false );
        m_actionPBS->setEnabled( false );
    }
}

void SkinningWidget::onSkinningChanged( int newType ) {
    CORE_ASSERT( m_current, "should be disabled" );
    CORE_ASSERT( newType >= 0 && newType <= 3, "Invalid Skinning Type" );
    m_current->setSkinningType( SkinningComponent::SkinningType( newType ) );
    switch ( newType )
    {
    case 0:
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB_on.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR.png" ) );
        m_actionPBS->setIcon( QIcon( ":/Assets/Images/PBS.png" ) );
        m_stackedWidget->setCurrentIndex( 0 );
        break;
    case 1:
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ_on.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR.png" ) );
        m_actionPBS->setIcon( QIcon( ":/Assets/Images/PBS.png" ) );
        m_stackedWidget->setCurrentIndex( 0 );
        break;
    case 2:
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR_on.png" ) );
        m_actionPBS->setIcon( QIcon( ":/Assets/Images/PBS.png" ) );
        m_stackedWidget->setCurrentIndex( 0 );
        break;
    case 3:
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR.png" ) );
        m_actionPBS->setIcon( QIcon( ":/Assets/Images/PBS_on.png" ) );
        m_stackedWidget->setCurrentIndex( 1 );
        break;
    default:
        break;
    }
}
} // namespace SkinningPlugin
