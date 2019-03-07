#include <Gui/MaterialEditor.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <QCloseEvent>

namespace Ra {
namespace Gui {
MaterialEditor::MaterialEditor( QWidget* parent ) :
    QWidget( parent ),
    m_visible( false ),
    m_roIdx( -1 ),
    m_usable( false ) {
    setupUi( this );
    typedef void ( QSpinBox::*sigPtr )( int );
    connect( kdR, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKdColorChanged );
    connect( kdG, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKdColorChanged );
    connect( kdB, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKdColorChanged );

    connect( ksR, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKsColorChanged );
    connect( ksG, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKsColorChanged );
    connect( ksB, static_cast<sigPtr>( &QSpinBox::valueChanged ), this,
             &MaterialEditor::onKsColorChanged );

    connect( exp,
             static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ),
             this, &MaterialEditor::onExpChanged );

    connect( kdColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKdColor );
    connect( ksColorWidget, &ColorWidget::newColorPicked, this, &MaterialEditor::newKsColor );

    setWindowModality( Qt::ApplicationModal );
    setWindowTitle( "Material Editor" );
}

void MaterialEditor::onExpChanged( double v ) {
    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_ns = Scalar( v );
    }
}

void MaterialEditor::onKdColorChanged( int ) {
    kdColorWidget->colorChanged( kdR->value(), kdG->value(), kdB->value() );

    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_kd = Core::Utils::Color (
              kdR->value() / 255_ra, kdG->value() / 255_ra, kdB->value() / 255_ra, 1_ra );

    }
}

void MaterialEditor::onKsColorChanged( int ) {
    ksColorWidget->colorChanged( ksR->value(), ksG->value(), ksB->value() );

    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_ks = Core::Utils::Color (
              ksR->value() / 255_ra, ksG->value() / 255_ra, ksB->value() / 255_ra, 1_ra );
    }
}

void MaterialEditor::newKdColor( const QColor& color ) {
    const QSignalBlocker br( kdR );
    const QSignalBlocker bg( kdG );
    const QSignalBlocker bb( kdB );

    kdR->setValue( color.red() );
    kdG->setValue( color.green() );
    kdB->setValue( color.blue() );

    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_kd = Core::Utils::Color (
              color.redF(), color.greenF(), color.blueF(), 1. );
    }
}

void MaterialEditor::newKsColor( const QColor& color ) {
    const QSignalBlocker br( ksR );
    const QSignalBlocker bg( ksG );
    const QSignalBlocker bb( ksB );

    ksR->setValue( color.red() );
    ksG->setValue( color.green() );
    ksB->setValue( color.blue() );

    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_ks = Core::Utils::Color (
              color.redF(), color.greenF(), color.blueF(), 1. );
    }
}

void MaterialEditor::showEvent( QShowEvent* e ) {
    QWidget::showEvent( e );

    m_visible = true;
}

void MaterialEditor::closeEvent( QCloseEvent* /*e*/ ) {
    m_visible = false;

    hide();
}

void MaterialEditor::changeRenderObject( Core::Utils::Index roIdx ) {
    if ( roIdx.isValid() )
    {
        m_renderObject =
            Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( roIdx );

        if ( m_renderObject != nullptr )
        {
            m_BlinnPhongGroup->hide();

            auto genericMaterial = m_renderObject->getRenderTechnique()->getMaterial();
            if ( genericMaterial->getMaterialName() == "BlinnPhong" )
            {
                m_blinnphongmaterial =
                    dynamic_cast<Ra::Engine::BlinnPhongMaterial*>( genericMaterial.get() );
                updateBlinnPhongViz();
                m_BlinnPhongGroup->show();
            }

            m_usable = true;
            m_roIdx = roIdx;
            m_renderObjectName->setText( m_renderObject->getName().c_str() );
        }
    }
}

void MaterialEditor::updateBlinnPhongViz() {
    const Core::Utils::Color kd = m_blinnphongmaterial->m_kd;
    const Core::Utils::Color ks = m_blinnphongmaterial->m_ks;

    int kdr = int( std::floor( kd.x() * 255_ra ) ),
        kdg = int( std::floor( kd.y() * 255_ra ) ),
        kdb = int( std::floor( kd.z() * 255_ra ) );
    int ksr = int( std::floor( ks.x() * 255_ra ) ),
        ksg = int( std::floor( ks.y() * 255_ra ) ),
        ksb = int( std::floor( ks.z() * 255_ra ) );

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

    exp->setValue( double( m_blinnphongmaterial->m_ns ) );

    kdColorWidget->colorChanged( kdr, kdg, kdb );
    ksColorWidget->colorChanged( ksr, ksg, ksb );
}

} // namespace Gui
} // namespace Ra

void Ra::Gui::MaterialEditor::on_m_closeButton_clicked() {
    hide();
}

void Ra::Gui::MaterialEditor::on_kUsePerVertex_clicked(bool checked)
{
    if ( m_renderObject && m_usable )
    {
        m_blinnphongmaterial->m_hasPerVertexKd = checked;
    }

}
