
#include <Gui/Widgets/ConstrainedNumericSpinBox.hpp>

namespace Ra {
namespace Gui {
namespace Widgets {

template <typename T>
QValidator::State ConstrainedNumericSpinBox<T>::validate( QString& input, int& ) const {
    auto valid = this->isValid( this->valueFromText( input ) );
    auto& spin = const_cast<ConstrainedNumericSpinBox&>( *this );
    spin.blockSignals( !valid );
    if ( valid ) { spin.setStyleSheet( "" ); }
    else {
        spin.setStyleSheet( "background-color: #FF8080" );
    }
    return valid ? QValidator::Acceptable : QValidator::Invalid;
}

} // namespace Widgets
} // namespace Gui
} // namespace Ra
