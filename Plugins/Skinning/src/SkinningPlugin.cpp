#include <SkinningPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <SkinningSystem.hpp>
#include <GuiBase/SelectionManager/SelectionManager.hpp>

namespace SkinningPlugin
{

    SkinningPluginC::~SkinningPluginC()
    {
    }

    void SkinningPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        m_system = new SkinningSystem;
        m_selectionManager = context.m_selectionManager;
        context.m_engine->registerSystem( "SkinningSystem", m_system );
        m_widget = new SkinningWidget;
        connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this, &SkinningPluginC::onCurrentChanged );
    }

    bool SkinningPluginC::doAddWidget( QString &name )
    {
        name = "Skinning";
        return true;
    }

    QWidget* SkinningPluginC::getWidget()
    {
        return m_widget;
    }

    bool SkinningPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* SkinningPluginC::getMenu()
    {
        return nullptr;
    }

    bool SkinningPluginC::doAddAction( int& nb )
    {
        nb = 0;
        return false;
    }

    QAction* SkinningPluginC::getAction( int id )
    {
        return nullptr;
    }

    void SkinningPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev )
    {
        Ra::Engine::ItemEntry it = m_selectionManager->currentItem();
        if (it.isComponentNode() || it.isRoNode() )
        {
            if ( it.m_component->getSystem() == m_system)
            {
                m_widget->setCurrent(it, static_cast<SkinningComponent*>(it.m_component));
            }
            else
            {
                m_widget->setCurrent(it, nullptr);
            }
        }
    }

    SkinningWidget::SkinningWidget( QWidget * parent ) : QFrame( parent ),
        m_current( nullptr )
    {
        m_skinningSelect = new QComboBox( this );
        m_skinningSelect->setMaxVisibleItems( 3 );
        m_skinningSelect->setMaxCount( 3 );
        m_skinningSelect->setDuplicatesEnabled( false );
        m_skinningSelect->setCurrentIndex( 1 );

        m_skinningSelect->insertItems( 0, QStringList()
            << "Linear Blend Skinning" << "Dual Quaternion Skinning" << "Center of Rotation skinning" );
        m_skinningSelect->setEnabled( false );

        connect( m_skinningSelect,
            static_cast< void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged),
            this,
            &SkinningWidget::onSkinningChanged );

    }

    void SkinningWidget::setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp )
    {
        m_current = comp;
        if ( comp )
        {
            CORE_ASSERT( entry.m_component == comp, "Component Inconsistency" );
            m_skinningSelect->setEnabled( true );
            m_skinningSelect->setCurrentIndex( int( comp->getSkinningType() ) );
        }
        else
        {
            m_skinningSelect->setEnabled( false );
        }
    }

    void SkinningWidget::onSkinningChanged( int newType )
    {
        CORE_ASSERT( m_current, "should be disabled" );
        CORE_ASSERT( newType >= 0 && newType < 3, "Invalid Skinning Type" );
        m_current->setSkinningType( SkinningComponent::SkinningType( newType ) );
    }

}
