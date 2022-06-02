#include <Gui/Widgets/CheckingSpinBox.hpp>
#include <QBoxLayout>
#include <algorithm>
#include <qspinbox.h>

namespace Ra {
namespace Gui {
namespace Widgets {

CheckingSpinBox::CheckingSpinBox( QWidget* parent ) : QWidget( parent ) {
    m_spinbox           = new QDoubleSpinBox();
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );
    layout->addWidget( m_spinbox );
    m_value = m_spinbox->value();
    connect( m_spinbox, SIGNAL( valueChanged( double ) ), SLOT( spinboxValueChanged( double ) ) );
}

double CheckingSpinBox::value() const {
    return m_value;
}

double CheckingSpinBox::singleStep() const {
    return m_spinbox->singleStep();
}

void CheckingSpinBox::setSingleStep( double val ) {
    m_spinbox->setSingleStep( val );
}

void CheckingSpinBox::setPredicate( std::function<bool( double )> pred ) {
    m_predicate = pred;
    if ( !m_predicate( m_value ) ) { m_spinbox->setStyleSheet( "background-color: #FF8080" ); }
}

int CheckingSpinBox::decimals() const {
    return m_spinbox->decimals();
}

void CheckingSpinBox::setDecimals( int prec ) {
    m_spinbox->setDecimals( prec );
}

void CheckingSpinBox::setValue( double val ) {
    if ( m_predicate( val ) ) {
        m_spinbox->setValue( val );
        m_value = val;
    }
}

void CheckingSpinBox::spinboxValueChanged( double val ) {
    if ( m_predicate( val ) ) {
        m_spinbox->setStyleSheet( "" );
        m_value = val;
        emit valueChanged( val );
    }
    else {
        m_spinbox->setStyleSheet( "background-color: #FF8080" );
    }
}

} // namespace Widgets
} // namespace Gui
} // namespace Ra
