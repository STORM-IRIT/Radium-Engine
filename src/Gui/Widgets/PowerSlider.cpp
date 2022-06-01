#include <Gui/Widgets/PowerSlider.hpp>

#include <QHBoxLayout>
#include <cmath>
#include <qslider.h>
#include <qspinbox.h>

#include <limits>
namespace Ra::Gui::Widgets {
PowerSlider::PowerSlider( QWidget* parent, double alignmentValue ) : QWidget( parent ) {
    // create widget
    slider_  = new QSlider( Qt::Horizontal, this );
    spinBox_ = new QDoubleSpinBox( this );

    // style question
    spinBox_->setAlignment( Qt::AlignRight );
    spinBox_->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    // minimum size for "nice" alignment
    spinBox_->setRange( 0, alignmentValue );
    spinBox_->setMinimumSize( QSize( spinBox_->sizeHint().width(), 0 ) );

    // update slider range
    slider_->setRange( 0, 100000 );

    // default value
    setRange( 0.0, 1.0 );
    setSingleStep( 0.1 );

    // connect spinBox_ and slider_
    connect(
        spinBox_, SIGNAL( valueChanged( double ) ), SLOT( on_spinBox_valueChanged( double ) ) );
    connect( slider_, SIGNAL( valueChanged( int ) ), SLOT( on_slider_valueChanged( int ) ) );

    // connect to external signal
    connect( spinBox_, SIGNAL( valueChanged( double ) ), SIGNAL( valueChanged( double ) ) );

    // layout in a hbox
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 6 );
    layout->addWidget( slider_ );
    layout->addWidget( spinBox_ );
    setLayout( layout );
}

PowerSlider::~PowerSlider() {
    disconnect( spinBox_, 0, 0, 0 );
    disconnect( slider_, 0, 0, 0 );
    disconnect( this, 0, 0, 0 );

    delete spinBox_;
    delete slider_;
    delete layout();
}

void PowerSlider::on_slider_valueChanged( int val ) {
    // compute corresponding value in spinBox_ space
    double dval = spinBox_->minimum() + double( val - slider_->minimum() ) /
                                            double( slider_->maximum() - slider_->minimum() ) *
                                            ( spinBox_->maximum() - spinBox_->minimum() );

    // update if needed
    if ( fabs( dval - spinBox_->value() ) >= std::numeric_limits<float>::epsilon() ) {
        spinBox_->setValue( dval );
    }
}

void PowerSlider::on_spinBox_valueChanged( double val ) {
    // compute corresponding value in slider_ space
    int ival =
        int( ( ( val - spinBox_->minimum() ) / ( spinBox_->maximum() - spinBox_->minimum() ) *
               ( slider_->maximum() - slider_->minimum() ) ) );

    // update if needed
    if ( ival != slider_->value() ) { slider_->setValue( ival ); }
}

void PowerSlider::setRange( double min, double max ) {
    int decimals = std::max( int( round( std::abs( log( min - floor( min ) ) / log( 10 ) ) ) ),
                             int( round( std::abs( log( max - floor( max ) ) / log( 10 ) ) ) ) ) +
                   1;
    spinBox_->setDecimals( decimals );
    spinBox_->setRange( min, max );

    setSingleStep( std::min( spinBox_->singleStep(), ( max - min ) / 10.0 ) );
}

void PowerSlider::setMinimum( double min ) {
    double max = spinBox_->maximum();
    if ( min > max ) { max = min; }
    setRange( min, max );
}

void PowerSlider::setMaximum( double max ) {
    double min = spinBox_->minimum();
    if ( max < min ) { min = max; }
    setRange( min, max );
}

void PowerSlider::setSingleStep( double step ) {

    int decimals = int( round( std::abs( log( step - floor( step ) ) / log( 10 ) ) ) ) + 1;
    spinBox_->setDecimals( decimals );

    // set spinBox_ step
    spinBox_->setSingleStep( step );

    // set slider step with the same relative value
    slider_->setSingleStep( int( step / ( spinBox_->maximum() - spinBox_->minimum() ) *
                                 double( slider_->maximum() - slider_->minimum() ) ) );
}

void PowerSlider::setValue( double dval ) {
    if ( fabs( dval - spinBox_->value() ) >= std::numeric_limits<float>::epsilon() ) {
        spinBox_->setValue( dval );
    }
}
#ifdef ENABLE_QDOM
QDomElement PowerSlider::domElement( const QString& name, QDomDocument& document ) const {
    QDomElement de = document.createElement( name );
    de.setAttribute( "value", QString::number( spinBox_->value() ) );
    return de;
}

void PowerSlider::initFromDOMElement( const QDomElement& e ) {
    float value = spinBox_->minimum();
    if ( e.hasAttribute( "value" ) ) {
        const QString s = e.attribute( "value" );
        bool ok;
        s.toFloat( &ok );
        if ( ok )
            value = s.toFloat();
        else { // throw error ??
        }
    }
    else {
        // throw error ??
    }
    setValue( value );
}
#endif
double PowerSlider::value() {
    return spinBox_->value();
}

double PowerSlider::minimum() {
    return spinBox_->minimum();
}
double PowerSlider::maximum() {
    return spinBox_->maximum();
}
double PowerSlider::singleStep() {
    return spinBox_->singleStep();
}
} // namespace Ra::Gui::Widgets
