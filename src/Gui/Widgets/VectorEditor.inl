#pragma once

#include <Gui/Widgets/VectorEditor.inl>

#include <Gui/Widgets/QtTypeWrapper.hpp>

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

namespace Ra {
namespace Gui {
namespace Widgets {

template <typename T>
VectorEditor<T>::VectorEditor( const std::vector<T>& vector, QWidget* parent ) :
    VectorEditorSignals( parent ), m_vector( vector ) {

    QVBoxLayout* layout = new QVBoxLayout;

    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 6 );
    for ( auto& elem : m_vector ) {
        auto spinbox = new WidgetType( this );
        spinbox->setMinimum( std::numeric_limits<T>::lowest() );
        spinbox->setMaximum( std::numeric_limits<T>::max() );
        spinbox->setValue( elem );
        if constexpr ( std::is_floating_point_v<T> ) { spinbox->setDecimals( 3 ); }
        layout->addWidget( spinbox );
        auto updateVectorOnChange = [this, &elem]( SignalType value ) {
            elem = T( value );
            emit valueChanged( m_vector );
        };
        connect(
            spinbox, QOverload<SignalType>::of( &WidgetType::valueChanged ), updateVectorOnChange );
    }
    setLayout( layout );
}

template <typename T>
const std::vector<T>& VectorEditor<T>::vector() const {
    return m_vector;
}
} // namespace Widgets
} // namespace Gui
} // namespace Ra
