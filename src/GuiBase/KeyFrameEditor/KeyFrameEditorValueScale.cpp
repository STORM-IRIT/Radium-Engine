#include <GuiBase/KeyFrameEditor/KeyFrameEditorValueScale.h>

#include <QPainter>
#include <QScrollBar>

#include <GuiBase/KeyFrameEditor/KeyFrameEditorScrollArea.h>

namespace Ra::GuiBase {

KeyframeEditorValueScale::KeyframeEditorValueScale( QWidget* parent ) : QFrame( parent ) {}

void KeyframeEditorValueScale::setScrollArea( KeyFrameEditorScrollArea* scrollArea ) {
    m_scrollArea = scrollArea;
}

void KeyframeEditorValueScale::paintEvent( QPaintEvent* ) {
    if ( m_scrollArea == nullptr ) { return; }

    QPainter painter( this );
    painter.setRenderHint( QPainter::HighQualityAntialiasing );

    int offset = m_scrollArea->verticalScrollBar()->value();

    Scalar pixPerValue = m_scrollArea->getPixPerValue();
    Scalar stepValue   = m_scrollArea->getStepValue();
    Scalar maxValue    = m_scrollArea->getMaxValue();
    for ( int i = 1; i < m_scrollArea->getNbIntervalValue(); i++ )
    {
        int y         = int( pixPerValue * stepValue * i );
        QString value = QString::number( double( maxValue / 2 - ( i - 1 ) * stepValue ) );
        painter.drawText( 5, y - offset, value );
    }
}

} // namespace Ra::GuiBase
