#include <GuiBase/Timeline/qframetimescale.h>

#include <QPainter>

#include <GuiBase/Timeline/qwidgetruler.h>

namespace Ra::GuiBase {

QFrameTimescale::QFrameTimescale( QWidget* parent ) : QFrame( parent ) {
    m_widgetRuler = static_cast<QWidgetRuler*>( parent );

    m_nbInterval = m_widgetRuler->getNbInterval();
    m_step       = m_widgetRuler->getStep();
    m_pixPerSec  = m_widgetRuler->getPixPerSec();

    m_drawLock = m_widgetRuler->getTimescaleLock();
}

void QFrameTimescale::paintEvent( QPaintEvent* ) {

    QPainter painter( this );
    painter.setRenderHint( QPainter::HighQualityAntialiasing );

    painter.drawText( 0, 11, "sec" );
    for ( int i = 1; i < *m_nbInterval; i++ )
    {
        int x        = static_cast<int>( *m_pixPerSec * *m_step * i );
        QString time = QString::number( ( i - 1 ) * *m_step );
        int dec      = time.size() * 6 / 2;
        painter.drawText( x - dec, 11, time );
    }
}

void QFrameTimescale::setDrawLock( bool* value ) {
    m_drawLock = value;
}

} // namespace Ra::GuiBase
