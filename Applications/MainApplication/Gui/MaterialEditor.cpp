#include <Gui/MaterialEditor.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <QCloseEvent>

namespace Ra
{
    namespace Gui
    {
        MaterialEditor::MaterialEditor( QWidget* parent )
            : QWidget( parent )
            , m_visible( false )
            , m_roIdx( -1 )
            , m_usable( false )
        {
            setupUi( this );
            typedef void ( QSpinBox::*sigPtr )( int );
            connect( kdR, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );
            connect( kdG, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );
            connect( kdB, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );

            connect( ksR, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );
            connect( ksG, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );
            connect( ksB, static_cast<sigPtr>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );

            connect( exp, static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ), this, &MaterialEditor::onExpChanged );

            connect( kdColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKdColor );
            connect( ksColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKsColor );
        }

        void MaterialEditor::onExpChanged( double v )
        {
            if ( m_renderObject && m_usable )
            {
                m_material->m_ns = v;
            }
        }

        void MaterialEditor::onKdColorChanged( int )
        {
            kdColorWidget->colorChanged( kdR->value(), kdG->value(), kdB->value() );

            if ( m_renderObject && m_usable )
            {
                Core::Color c( kdR->value() / 255.f, kdG->value() / 255.f, kdB->value() / 255.f, 1.0 );
                m_material->m_kd = c;
            }
        }

        void MaterialEditor::onKsColorChanged( int )
        {
            ksColorWidget->colorChanged( ksR->value(), ksG->value(), ksB->value() );

            if ( m_renderObject && m_usable )
            {
                Core::Color c( ksR->value() / 255.f, ksG->value() / 255.f, ksB->value() / 255.f, 1.0 );
                m_material->m_ks = c;
            }
        }

        void MaterialEditor::newKdColor( const QColor& color )
        {
            const QSignalBlocker br( kdR );
            const QSignalBlocker bg( kdG );
            const QSignalBlocker bb( kdB );

            kdR->setValue( color.red() );
            kdG->setValue( color.green() );
            kdB->setValue( color.blue() );

            if ( m_renderObject && m_usable)
            {
                Core::Color c( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_material->m_kd = c;
            }
        }

        void MaterialEditor::newKsColor( const QColor& color )
        {
            const QSignalBlocker br( ksR );
            const QSignalBlocker bg( ksG );
            const QSignalBlocker bb( ksB );

            ksR->setValue( color.red() );
            ksG->setValue( color.green() );
            ksB->setValue( color.blue() );

            if ( m_renderObject && m_usable )
            {
                Core::Color c( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_material->m_ks = c;
            }
        }

        void MaterialEditor::showEvent( QShowEvent* e )
        {
            QWidget::showEvent( e );

            m_visible = true;

            m_engine = Engine::RadiumEngine::getInstance();
            m_roMgr = m_engine->getRenderObjectManager();
        }

        void MaterialEditor::closeEvent( QCloseEvent* e )
        {
            m_visible = false;

            hide();
        }

        void MaterialEditor::changeRenderObject( Core::Index roIdx )
        {
            if ( !m_visible )
            {
                return;
            }

            m_roIdx = roIdx;
            m_renderObject = m_roMgr->getRenderObject( m_roIdx );

            /// TODO : replace this ugly dynamic_cast by something more static ...
            m_material = dynamic_cast<Ra::Engine::BlinnPhongMaterial *>( m_renderObject->getRenderTechnique()->getMaterial().get() );
            if ( m_material == nullptr )
            {
                m_usable = false;
                return;
            }

            if ( m_renderObject != nullptr )
            {
                m_renderObjectName->setText( m_renderObject->getName().c_str() );
                updateMaterialViz( );
            }
        }

        void MaterialEditor::updateMaterialViz( )
        {
            const Core::Color kd = m_material->m_kd;
            const Core::Color ks = m_material->m_ks;

            int kdr = kd.x() * 255, kdg = kd.y() * 255, kdb = kd.y() * 255;
            int ksr = ks.x() * 255, ksg = ks.y() * 255, ksb = ks.z() * 255;

            const QSignalBlocker bdr( kdR );
            const QSignalBlocker bdg( kdG );
            const QSignalBlocker bdb( kdB );

            const QSignalBlocker bsr( ksR );
            const QSignalBlocker bsg( ksG );
            const QSignalBlocker bsb( ksB );

            const QSignalBlocker bns( exp );

            kdR->setValue( kdr );
            kdG->setValue( kdg );
            kdB->setValue( kdb );

            ksR->setValue( ksr );
            ksG->setValue( ksg );
            ksB->setValue( ksb );

            exp->setValue( m_material->m_ns );

            kdColorWidget->colorChanged( kdr, kdg, kdb );
            ksColorWidget->colorChanged( ksr, ksg, ksb );
        }

    }
}
