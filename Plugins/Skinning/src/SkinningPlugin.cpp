#include <SkinningPlugin.hpp>

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <Core/Resources/Resources.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <SkinningSystem.hpp>

namespace SkinningPlugin {

SkinningPluginC::~SkinningPluginC() {}

void SkinningPluginC::registerPlugin( const Ra::Plugins::Context& context ) {
    m_system           = new SkinningSystem;
    m_selectionManager = context.m_selectionManager;
    context.m_engine->registerSystem( "SkinningSystem", m_system );
    m_widget = new SkinningWidget;
    if (m_selectionManager) {
        connect( m_selectionManager,
                 &Ra::GuiBase::SelectionManager::currentChanged,
                 this,
                 &SkinningPluginC::onCurrentChanged );
    }
    connect( m_widget, &SkinningWidget::showWeights, this, &SkinningPluginC::onShowWeights );
    connect(
        m_widget, &SkinningWidget::showWeightsType, this, &SkinningPluginC::onShowWeightsType );

    connect( m_widget, &SkinningWidget::askForUpdate, this, &SkinningPluginC::askForUpdate );
    connect( this, &SkinningPluginC::askForUpdate, &context, &Ra::Plugins::Context::askForUpdate );
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

bool SkinningPluginC::doAddROpenGLInitializer() {
    return m_system != nullptr;
}

void SkinningPluginC::openGlInitialize( const Ra::Plugins::Context& /*context*/ ) {
    if ( !m_system ) { return; }
    Ra::Engine::TextureParameters texData;
    texData.wrapS     = GL_CLAMP_TO_EDGE;
    texData.wrapT     = GL_CLAMP_TO_EDGE;
    texData.minFilter = GL_NEAREST;
    texData.magFilter = GL_NEAREST;
    // TODO : move Influence0 to a skinning specific resources container
    texData.name      = std::string(Ra::Core::Resources::getBaseDir()) + "Assets/Textures/Influence0.png";
    Ra::Engine::TextureManager::getInstance()->getOrLoadTexture( texData );
}

void SkinningPluginC::onCurrentChanged( const QModelIndex& /*current*/,
                                        const QModelIndex& /*prev*/ ) {
    Ra::Engine::ItemEntry it = m_selectionManager->currentItem();
    if ( it.m_entity )
    {
        auto comps = m_system->getEntityComponents( it.m_entity );
        if ( comps.size() != 0 )
        {
            for ( auto& comp : comps )
            {
                auto skin = static_cast<SkinningPlugin::SkinningComponent*>( comp );
                m_widget->setCurrent( it, skin );

                using BoneMap = std::map<Ra::Core::Utils::Index, uint>;
                auto CM       = Ra::Engine::ComponentMessenger::getInstance();
                auto BM       = *CM->getterCallback<BoneMap>( it.m_entity, skin->m_contentsName )();
                auto b_it     = BM.find( it.m_roIndex );
                if ( b_it != BM.end() ) { skin->setWeightBone( b_it->second ); }
            }
        }
        else
        { m_widget->setCurrent( it, nullptr ); }
    }
    else
    { m_widget->setCurrent( it, nullptr ); }
    askForUpdate();
}

void SkinningPluginC::onShowWeights( bool on ) {
    m_system->showWeights( on );
    askForUpdate();
}

void SkinningPluginC::onShowWeightsType( int type ) {
    m_system->showWeightsType( type );
    askForUpdate();
}

// Class SkinningWidget

SkinningWidget::SkinningWidget( QWidget* parent ) : QFrame( parent ), m_current( nullptr ) {
    auto vL = new QVBoxLayout( this );
    auto hL = new QHBoxLayout( this );

    m_skinningSelect = new QComboBox( this );
    m_skinningSelect->setMaxVisibleItems( 5 );
    m_skinningSelect->setMaxCount( 5 );
    m_skinningSelect->setDuplicatesEnabled( false );
    m_skinningSelect->setCurrentIndex( 1 );

    m_skinningSelect->insertItems( 0,
                                   QStringList() << "Linear Blend Skinning"
                                                 << "Dual Quaternion Skinning"
                                                 << "Center of Rotation skinning"
                                                 << "STBS LBS"
                                                 << "STBD DQS" );
    m_skinningSelect->setEnabled( false );
    vL->addWidget( m_skinningSelect );
    vL->addLayout( hL );

    m_showWeights = new QCheckBox( this );
    m_showWeights->setText( "Show weights" );
    m_showWeights->setEnabled( false );
    hL->addWidget( m_showWeights );

    m_skinningWeights = new QComboBox( this );
    m_skinningWeights->setMaxVisibleItems( 2 );
    m_skinningWeights->setMaxCount( 2 );
    m_skinningWeights->setDuplicatesEnabled( false );
    m_skinningWeights->setCurrentIndex( 1 );
    m_skinningWeights->insertItems( 0,
                                    QStringList() << "Standard weights"
                                                  << "STBS weights" );
    m_skinningWeights->setEnabled( false );
    hL->addWidget( m_skinningWeights );

    vL->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

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
             static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this,
             &SkinningWidget::onSkinningChanged );
    connect( m_showWeights,
             static_cast<void ( QCheckBox::* )( bool )>( &QCheckBox::toggled ),
             this,
             &SkinningWidget::onShowWeightsToggled );
    connect( m_skinningWeights,
             static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this,
             &SkinningWidget::onSkinningWeightsChanged );

    connect( m_actionLBS, &QAction::triggered, this, &SkinningWidget::onLSBActionTriggered );
    connect( m_actionDQ, &QAction::triggered, this, &SkinningWidget::onDQActionTriggered );
    connect( m_actionCoR, &QAction::triggered, this, &SkinningWidget::onCoRActionTriggered );
    connect(
        m_actionSTBSLBS, &QAction::triggered, this, &SkinningWidget::onSTBSLBSActionTriggered );
    connect(
        m_actionSTBSDQS, &QAction::triggered, this, &SkinningWidget::onSTBSDQSActionTriggered );
}

void SkinningWidget::onLSBActionTriggered() {
    m_skinningSelect->setCurrentIndex( 0 );
    m_actionLBS->setChecked( true );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
    askForUpdate();
}

void SkinningWidget::onDQActionTriggered() {
    m_skinningSelect->setCurrentIndex( 1 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( true );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
    askForUpdate();
}

void SkinningWidget::onCoRActionTriggered() {
    m_skinningSelect->setCurrentIndex( 2 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( true );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( false );
    askForUpdate();
}

void SkinningWidget::onSTBSLBSActionTriggered() {
    m_skinningSelect->setCurrentIndex( 3 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( true );
    m_actionSTBSDQS->setChecked( false );
    askForUpdate();
}

void SkinningWidget::onSTBSDQSActionTriggered() {
    m_skinningSelect->setCurrentIndex( 4 );
    m_actionLBS->setChecked( false );
    m_actionDQ->setChecked( false );
    m_actionCoR->setChecked( false );
    m_actionSTBSLBS->setChecked( false );
    m_actionSTBSDQS->setChecked( true );
    askForUpdate();
}

void SkinningWidget::setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp ) {
    m_current = comp;
    if ( comp )
    {
        m_skinningSelect->setEnabled( true );
        m_showWeights->setEnabled( true );
        m_skinningWeights->setEnabled( true );
        m_actionLBS->setEnabled( true );
        m_actionDQ->setEnabled( true );
        m_actionCoR->setEnabled( true );
        m_actionSTBSLBS->setEnabled( true );
        m_actionSTBSDQS->setEnabled( true );
        m_skinningSelect->setCurrentIndex( int( comp->getSkinningType() ) );
    }
    else
    {
        m_skinningSelect->setEnabled( false );
        m_showWeights->setEnabled( false );
        m_skinningWeights->setEnabled( false );
        m_actionLBS->setEnabled( false );
        m_actionDQ->setEnabled( false );
        m_actionCoR->setEnabled( false );
        m_actionSTBSLBS->setEnabled( false );
        m_actionSTBSDQS->setEnabled( false );
    }
    askForUpdate();
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
    askForUpdate();
}

void SkinningWidget::onShowWeightsToggled( bool on ) {
    emit showWeights( on );
}

void SkinningWidget::onSkinningWeightsChanged( int newType ) {
    emit showWeightsType( newType );
}

} // namespace SkinningPlugin
