#include <GuiBase/Timeline/qframebuttons.h>

#include <QMessageBox>
#include <QWheelEvent>

#include <GuiBase/Timeline/Timeline.h>
#include <GuiBase/Timeline/qwidgetruler.h>

namespace Ra::GuiBase {

QFrameButtons::QFrameButtons( QWidget* parent ) : QFrame( parent ) {}

void QFrameButtons::setRuler( QWidgetRuler* value ) {
    m_ruler = value;
}

void QFrameButtons::keyPressEvent( QKeyEvent* event ) {
    emit keyPressed( event );
}

void QFrameButtons::keyReleaseEvent( QKeyEvent* event ) {
    emit keyReleased( event );
}

void QFrameButtons::setTimeline( Timeline* value ) {
    m_timeline = value;
}

void QFrameButtons::helpClicked() {
    QMessageBox msgBox;
    msgBox.setText( m_helpButton->toolTip() );
    msgBox.exec();
}

void QFrameButtons::setHelpButton( QToolButton* value ) {
    m_helpButton = value;
}

void QFrameButtons::setScrollArea( QScrollArea* value ) {
    m_scrollArea = value;
}

} // namespace Ra::GuiBase
