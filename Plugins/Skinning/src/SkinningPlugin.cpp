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
        nb = 3;
        return true;
    }

    QAction* SkinningPluginC::getAction( int id )
    {
        switch (id){
        case 0:
        {
            return m_widget->m_actionLBS;
        }
        case 1:
        {
            return m_widget->m_actionDQ;
        }
        case 2:
        {
            return m_widget->m_actionCoR;
        }
        default:
            return nullptr;
        }
    }

    void SkinningPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev )
    {
        Ra::Engine::ItemEntry it = m_selectionManager->currentItem();
        if (it.m_entity)
        {
            auto comps = m_system->getEntityComponents( it.m_entity );
            if (comps.size() != 0)
            {
                m_widget->setCurrent(it, static_cast<SkinningPlugin::SkinningComponent*>(comps[0]));
            }
            else
            {
                m_widget->setCurrent(it, nullptr);
            }
        } else {
            m_widget->setCurrent(it, nullptr);
        }
    }


    // Class SkinningWidget


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

        m_actionLBS = new QAction(QIcon(":/Assets/Images/LB.png"), QString("Linear Blending"),nullptr);
        m_actionDQ  = new QAction(QIcon(":/Assets/Images/DQ_on.png"), QString("Dual Quaternion"), nullptr);
        m_actionCoR = new QAction(QIcon(":/Assets/Images/CoR.png"), QString("Center of Rotation"),nullptr);
        m_actionLBS->setEnabled( false );
        m_actionDQ->setEnabled( false );
        m_actionCoR->setEnabled( false );

        connect( m_skinningSelect,
            static_cast< void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged),
            this,
            &SkinningWidget::onSkinningChanged );

        connect( m_actionLBS, &QAction::triggered, this, &SkinningWidget::onLSBActionTriggered );
        connect( m_actionDQ,  &QAction::triggered, this, &SkinningWidget::onDQActionTriggered );
        connect( m_actionCoR, &QAction::triggered, this, &SkinningWidget::onCoRActionTriggered );
    }


    void SkinningWidget::onLSBActionTriggered()
    {
        m_skinningSelect->setCurrentIndex(0);
    }

    void SkinningWidget::onDQActionTriggered()
    {
        m_skinningSelect->setCurrentIndex(1);
    }

    void SkinningWidget::onCoRActionTriggered()
    {
        m_skinningSelect->setCurrentIndex(2);
    }

    void SkinningWidget::setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp )
    {
        m_current = comp;
        if ( comp )
        {
            m_skinningSelect->setEnabled( true );
            m_actionLBS->setEnabled( true );
            m_actionDQ->setEnabled( true );
            m_actionCoR->setEnabled( true );
            m_skinningSelect->setCurrentIndex( int( comp->getSkinningType() ) );
        }
        else
        {
            m_skinningSelect->setEnabled( false );
            m_actionLBS->setEnabled( false );
            m_actionDQ->setEnabled( false );
            m_actionCoR->setEnabled( false );
        }
    }

    void SkinningWidget::onSkinningChanged( int newType )
    {
        CORE_ASSERT( m_current, "should be disabled" );
        CORE_ASSERT( newType >= 0 && newType < 3, "Invalid Skinning Type" );
        m_current->setSkinningType( SkinningComponent::SkinningType( newType ) );
        switch (newType) {
        case 0:
        {
            m_actionLBS->setIcon(QIcon(":/Assets/Images/LB_on.png"));
            m_actionDQ->setIcon(QIcon(":/Assets/Images/DQ.png"));
            m_actionCoR->setIcon(QIcon(":/Assets/Images/CoR.png"));
            break;
        }
        case 1:
        {
            m_actionLBS->setIcon(QIcon(":/Assets/Images/LB.png"));
            m_actionDQ->setIcon(QIcon(":/Assets/Images/DQ_on.png"));
            m_actionCoR->setIcon(QIcon(":/Assets/Images/CoR.png"));
            break;
        }
        case 2:
        {
            m_actionLBS->setIcon(QIcon(":/Assets/Images/LB.png"));
            m_actionDQ->setIcon(QIcon(":/Assets/Images/DQ.png"));
            m_actionCoR->setIcon(QIcon(":/Assets/Images/CoR_on.png"));
            break;
        }
        default:
        {
            break;
        }
        }
    }

}
