#include <GuiBase/Timeline/qwidgetruler.h>

#include <GuiBase/Timeline/Configurations.h>
#include <GuiBase/Timeline/qdoublespinboxsmart.h>

#include <QScrollBar>
#include <QWheelEvent>
#include <QtMath>

namespace Ra::GuiBase {

QWidgetRuler::QWidgetRuler( QWidget* parent ) : QWidget( parent ) {}

int QWidgetRuler::drawRuler( int width ) {
    int iStep = 0;
    while ( iStep < s_nbSteps && width * m_steps[iStep] < 50 * m_maxDuration )
        iStep++;

    if ( iStep == s_nbSteps ) { return this->width(); }

    m_step = m_steps[iStep];
    m_spinStart->setSingleStep( 0.5 * m_step );
    m_spinEnd->setSingleStep( 0.5 * m_step );
    m_spinCursor->setSingleStep( 0.5 * m_step );
    m_spinDuration->setSingleStep( 0.5 * m_step );

    m_nbInterval = qCeil( m_maxDuration / m_step ) + 2;
    m_pixPerSec  = ( width / double( m_nbInterval ) ) / m_step;

    m_zero = m_pixPerSec * m_step;
    setMinimumWidth( width );

    update();

    return width;
}

void QWidgetRuler::onDrawRuler( int width ) {
    drawRuler( width );
}

void QWidgetRuler::setCtrlDown( bool* value ) {
    m_ctrlDown = value;
}

void QWidgetRuler::setShiftDown( bool* value ) {
    m_shiftDown = value;
}

void QWidgetRuler::setSpinDuration( QDoubleSpinBoxSmart* value ) {
    m_spinDuration = value;
}

void QWidgetRuler::setSpinCursor( QDoubleSpinBoxSmart* value ) {
    m_spinCursor = value;
}

void QWidgetRuler::setSpinEnd( QDoubleSpinBoxSmart* value ) {
    m_spinEnd = value;
}

void QWidgetRuler::setSpinStart( QDoubleSpinBoxSmart* value ) {
    m_spinStart = value;
}

bool* QWidgetRuler::getSelectorLock() {
    return &m_selectorLock;
}

bool* QWidgetRuler::getTimescaleLock() {
    return &m_timescaleLock;
}

double* QWidgetRuler::getMaxDuration() {
    return &m_maxDuration;
}

double* QWidgetRuler::getZero() {
    return &m_zero;
}

double* QWidgetRuler::getPixPerSec() {
    return &m_pixPerSec;
}

int* QWidgetRuler::getNbInterval() {
    return &m_nbInterval;
}

double* QWidgetRuler::getStep() {
    return &m_step;
}

} // namespace Ra::GuiBase
