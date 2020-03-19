#include <GuiBase/KeyFrameEditor/KeyFrameEditorTimeScale.h>

#include <QPainter>
#include <QScrollBar>

#include <GuiBase/KeyFrameEditor/KeyFrameEditorScrollArea.h>

namespace Ra::GuiBase {

KeyFrameEditorTimeScale::KeyFrameEditorTimeScale( QWidget* parent ) : QFrame( parent ) {}

void KeyFrameEditorTimeScale::setScrollArea( KeyFrameEditorScrollArea* scrollArea ) {
    m_scrollArea = scrollArea;
}

void KeyFrameEditorTimeScale::paintEvent( QPaintEvent* ) {
    if ( m_scrollArea == nullptr ) { return; }

    QPainter painter( this );
    painter.setRenderHint( QPainter::HighQualityAntialiasing );

    int offset = m_scrollArea->horizontalScrollBar()->value();

    Scalar pixPerSec = m_scrollArea->getPixPerSec();
    Scalar stepTime  = m_scrollArea->getStepTime();
    for ( int i = 1; i < m_scrollArea->getNbIntervalTime(); i++ )
    {
        int x        = int( pixPerSec * stepTime * i );
        QString time = QString::number( double( ( i - 1 ) * stepTime ) );
        int dec      = time.size() * 3; // 6 / 2
        painter.drawText( x - dec - offset, 15, time );
    }
}

} // namespace Ra::GuiBase
