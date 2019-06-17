#include <Gui/ColorWidget.hpp>

#include <QColorDialog>
#include <QMouseEvent>
#include <QPalette>

#include <GuiBase/Utils/KeyMappingManager.hpp>

namespace Ra {
namespace Gui {

ColorWidget::ColorWidget( QWidget* widget ) : QWidget( widget ), m_currentColor( 0, 0, 0 ) {
    setAutoFillBackground( true );
    colorChanged( 0, 0, 0 );
}

void ColorWidget::colorChanged( int r, int g, int b ) {
    m_currentColor.setRgb( r, g, b );
    colorChanged();
}

void ColorWidget::colorChanged() {
    QPalette pal( m_currentColor );
    setPalette( pal );
    repaint();
}

void ColorWidget::mousePressEvent( QMouseEvent* /*event*/ ) {

        QColor color = QColorDialog::getColor( m_currentColor );
        if ( color != m_currentColor )
        {
            m_currentColor = color;
            colorChanged();

            emit newColorPicked( m_currentColor );
        }
}
} // namespace Gui
} // namespace Ra
