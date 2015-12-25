#include <MainApplication/Gui/MaterialEditor.hpp>

#include <QCloseEvent>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra
{
    namespace Gui
    {
        MaterialEditor::MaterialEditor( QWidget* parent )
            : QWidget( parent )
            , m_visible( false )
            , m_roIdx( -1 )
        {
            setupUi( this );

            connect( outputValue, static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ), this, &MaterialEditor::changeOutputValue );

            connect( kdR, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );
            connect( kdG, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );
            connect( kdB, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKdColorChanged );

            connect( ksR, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );
            connect( ksG, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );
            connect( ksB, static_cast<void ( QSpinBox::* )( int )>( &QSpinBox::valueChanged ), this, &MaterialEditor::onKsColorChanged );

            connect( exp, static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ), this, &MaterialEditor::onExpChanged );

            connect( kdColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKdColor );
            connect( ksColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKsColor );
        }

        void MaterialEditor::onExpChanged( double v )
        {
            if ( m_renderObject )
            {
                m_renderObject->getRenderTechnique()->material->setNs( v );
            }
        }

        void MaterialEditor::onKdColorChanged( int )
        {
            kdColorWidget->colorChanged( kdR->value(), kdG->value(), kdB->value() );

            if ( m_renderObject )
            {
                Core::Color c( kdR->value() / 255.f, kdG->value() / 255.f, kdB->value() / 255.f, 1.0 );
                m_renderObject->getRenderTechnique()->material->setKd( c );
            }
        }

        void MaterialEditor::onKsColorChanged( int )
        {
            ksColorWidget->colorChanged( ksR->value(), ksG->value(), ksB->value() );

            if ( m_renderObject )
            {
                Core::Color c( ksR->value() / 255.f, ksG->value() / 255.f, ksB->value() / 255.f, 1.0 );
                m_renderObject->getRenderTechnique()->material->setKs( c );
            }
        }

        void MaterialEditor::newKdColor( const QColor& color )
        {
            const QSignalBlocker br( ksR );
            const QSignalBlocker bg( ksG );
            const QSignalBlocker bb( ksB );

            kdR->setValue( color.red() );
            kdG->setValue( color.green() );
            kdB->setValue( color.blue() );

            if ( m_renderObject )
            {
                Core::Color c( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_renderObject->getRenderTechnique()->material->setKd( c );
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

            if ( m_renderObject )
            {
                Core::Color c( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_renderObject->getRenderTechnique()->material->setKs( c );
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

            if ( m_renderObject != nullptr )
            {
                m_renderObjectName->setText( m_renderObject->getName().c_str() );
                updateMaterialViz( m_renderObject->getRenderTechnique()->material );
            }
        }

        void MaterialEditor::updateMaterialViz( const Engine::Material* material )
        {
            const Core::Color kd = material->getKd();
            const Core::Color ks = material->getKs();

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

            exp->setValue( material->getNs() );

            kdColorWidget->colorChanged( kdr, kdg, kdb );
            ksColorWidget->colorChanged( ksr, ksg, ksb );
        }

        void MaterialEditor::changeOutputValue( int index )
        {
            if ( !m_visible || !m_renderObject )
            {
                return;
            }

            // TODO: Do this in deferred
            switch ( index )
            {
                case OUTPUT_FINAL:
                {
                    m_renderObject->getRenderParameters().updateParameter( "outputValue", 0 );
                } break;

                case OUTPUT_DIFFUSE:
                {
                    m_renderObject->getRenderParameters().updateParameter( "outputValue", 1 );
                } break;

                case OUTPUT_SPECULAR:
                {
                    m_renderObject->getRenderParameters().updateParameter( "outputValue", 2 );
                } break;

                case OUTPUT_NORMAL:
                {
                    m_renderObject->getRenderParameters().updateParameter( "outputValue", 3 );
                } break;
            }
        }


    }
}
