#include <Gui/ColorWidget.hpp>

#include <QPalette>
#include <QMouseEvent>
#include <QColorDialog>

#include <GuiBase/Utils/KeyMappingManager.hpp>

namespace Ra
{
    namespace Gui
    {

        ColorWidget::ColorWidget( QWidget* widget )
            : QWidget( widget )
            , m_currentColor( 0, 0, 0 )
        {
            setAutoFillBackground( true );
            colorChanged( 0, 0, 0 );
        }

        void ColorWidget::colorChanged( int r, int g, int b )
        {
            m_currentColor.setRgb( r, g, b );
            colorChanged();
        }

        void ColorWidget::colorChanged()
        {
            QPalette pal( m_currentColor );
            setPalette( pal );
            repaint();
        }

        void ColorWidget::mousePressEvent( QMouseEvent* e )
        {
            if ( KeyMappingManager::getInstance()->actionTriggered( e, KeyMappingManager::COLORWIDGET_PRESSBUTTON ) )
            {
                QColor color = QColorDialog::getColor( m_currentColor );
                if ( color != m_currentColor )
                {
                    m_currentColor = color;
                    colorChanged();

                    emit newColorPicked( m_currentColor );
                }
            }
        }
    }
}
