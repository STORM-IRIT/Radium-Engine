#include <SkinningPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <SkinningSystem.hpp>

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
    nb = 5;
    return true;
}

QAction* SkinningPluginC::getAction( int id ) {
    switch ( id )
    {
    case 0:
    { return m_widget->m_actionLBS; }
    case 1:
    { return m_widget->m_actionDQ; }
    case 2:
    { return m_widget->m_actionCoR; }
    case 3:
    { return m_widget->m_actionSTBSLBS; }
    case 4:
    { return m_widget->m_actionSTBSDQS; }
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
        {
            m_widget->setCurrent( it, static_cast<SkinningPlugin::SkinningComponent*>( comps[0] ) );
        } else
        { m_widget->setCurrent( it, nullptr ); }
    } else
    { m_widget->setCurrent( it, nullptr ); }
}

// Class SkinningWidget

SkinningWidget::SkinningWidget( QWidget* parent ) : QFrame( parent ), m_current( nullptr ) {
    m_skinningSelect = new QComboBox( this );
    m_skinningSelect->setMaxVisibleItems( 5 );
    m_skinningSelect->setMaxCount( 5 );
    m_skinningSelect->setDuplicatesEnabled( false );
    m_skinningSelect->setCurrentIndex( 1 );

    m_skinningSelect->insertItems( 0, QStringList() << "Linear Blend Skinning"
                                                    << "Dual Quaternion Skinning"
                                                    << "Center of Rotation skinning"
                                                    << "STBS LBS"
                                                    << "STBD DQS" );
    m_skinningSelect->setEnabled( false );

    m_actionLBS =
        new QAction( QIcon( ":/Assets/Images/LB.png" ), QString( "Linear Blending" ), nullptr );
    m_actionDQ =
        new QAction( QIcon( ":/Assets/Images/DQ_on.png" ), QString( "Dual Quaternion" ), nullptr );
    m_actionCoR =
        new QAction( QIcon( ":/Assets/Images/CoR.png" ), QString( "Center of Rotation" ), nullptr );
    m_actionSTBSLBS =
        new QAction( QIcon( ":/Assets/Images/LB.png" ), QString( "STBS with LBS" ), nullptr );
    m_actionSTBSDQS =
        new QAction( QIcon( ":/Assets/Images/DQ.png" ), QString( "STBS with DQS" ), nullptr );

    m_actionLBS->setEnabled( false );
    m_actionDQ->setEnabled( false );
    m_actionCoR->setEnabled( false );
    m_actionSTBSLBS->setEnabled( false );
    m_actionSTBSDQS->setEnabled( false );
    m_actionLBS->setCheckable( true );
    m_actionDQ->setCheckable( true );
    m_actionCoR->setCheckable( true );
    m_actionSTBSLBS->setCheckable( true );
    m_actionSTBSDQS->setCheckable( true );

    connect( m_skinningSelect,
             static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ), this,
             &SkinningWidget::onSkinningChanged );

    connect( m_actionLBS, &QAction::triggered, this, &SkinningWidget::onLSBActionTriggered );
    connect( m_actionDQ, &QAction::triggered, this, &SkinningWidget::onDQActionTriggered );
    connect( m_actionCoR, &QAction::triggered, this, &SkinningWidget::onCoRActionTriggered );
    connect( m_actionSTBSLBS, &QAction::triggered, this,
             &SkinningWidget::onSTBSLBSActionTriggered );
    connect( m_actionSTBSDQS, &QAction::triggered, this,
             &SkinningWidget::onSTBSDQSActionTriggered );
}

void SkinningWidget::onLSBActionTriggered() {
    m_skinningSelect->setCurrentIndex( 0 );
    m_actionLBS->setChecked( true );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
}

void SkinningWidget::onDQActionTriggered() {
    m_skinningSelect->setCurrentIndex( 1 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( true );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
}

void SkinningWidget::onCoRActionTriggered() {
    m_skinningSelect->setCurrentIndex( 2 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( true );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
}

void SkinningWidget::onSTBSLBSActionTriggered() {
    m_skinningSelect->setCurrentIndex( 3 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( true );
    m_actionSTBSDQS->setChecked( false );
}

void SkinningWidget::onSTBSDQSActionTriggered() {
    m_skinningSelect->setCurrentIndex( 4 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( true );
}

void SkinningWidget::setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp ) {
    m_current = comp;
    if ( comp )
    {
        m_skinningSelect->setEnabled( true );
        m_actionLBS->setEnabled( true );
        m_actionDQ->setEnabled( true );
        m_actionCoR->setEnabled( true );
        m_actionSTBSLBS->setEnabled( true );
        m_actionSTBSDQS->setEnabled( true );
        m_skinningSelect->setCurrentIndex( int( comp->getSkinningType() ) );
    } else
    {
        m_skinningSelect->setEnabled( false );
        m_actionLBS->setEnabled( false );
        m_actionDQ->setEnabled( false );
        m_actionCoR->setEnabled( false );
        m_actionSTBSLBS->setEnabled( false );
        m_actionSTBSDQS->setEnabled( false );
    }
}

void SkinningWidget::onSkinningChanged( int newType ) {
    CORE_ASSERT( m_current, "should be disabled" );
    CORE_ASSERT( newType >= 0 && newType < 5, "Invalid Skinning Type" );
    m_current->setSkinningType( SkinningComponent::SkinningType( newType ) );
    switch ( newType )
    {
    case 0:
    {
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB_on.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR.png" ) );
        break;
    }
    case 1:
    {
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ_on.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR.png" ) );
        break;
    }
    case 2:
    {
        m_actionLBS->setIcon( QIcon( ":/Assets/Images/LB.png" ) );
        m_actionDQ->setIcon( QIcon( ":/Assets/Images/DQ.png" ) );
        m_actionCoR->setIcon( QIcon( ":/Assets/Images/CoR_on.png" ) );
        break;
    }
    default:
    { break; }
    }
}

} // namespace SkinningPlugin
