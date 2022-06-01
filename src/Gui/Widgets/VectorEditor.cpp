#include "VectorEditor.hpp"
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

namespace Ra::Gui::Widgets {
VectorEditor::VectorEditor( const std::vector<double>& vector, int dec, QWidget* parent ) :
    QWidget( parent ), m_vector( vector ) {

    QVBoxLayout* layout = new QVBoxLayout;

    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 6 );
    for ( auto& elem : m_vector ) {
        auto spinbox = new QDoubleSpinBox();
        spinbox->setValue( elem );
        spinbox->setDecimals( dec );
        layout->addWidget( spinbox );
        auto updateVectorOnChange = [this, &elem]( double value ) {
            elem = value;
            emit valueChanged( m_vector );
        };
        connect(
            spinbox, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), updateVectorOnChange );
    }
    setLayout( layout );
}

const std::vector<double>& VectorEditor::vector() {
    return m_vector;
}
} // namespace Ra::Gui::Widgets
