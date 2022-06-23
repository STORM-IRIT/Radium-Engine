#include <Gui/Widgets/ControlPanel.hpp>
#include <QLabel>

#include <Gui/Widgets/CheckingSpinBox.hpp>

namespace Ra {
namespace Gui {
namespace Widgets {

template <typename T>
void ControlPanel::addCheckingNumberInput( const std::string& name,
                                           std::function<void( T )> callback,
                                           Scalar initial,
                                           std::function<bool( T )> predicate,
                                           const std::string& tooltip,
                                           int dec ) {
    auto inputLayout = new QHBoxLayout();
    auto inputLabel  = new QLabel( tr( name.c_str() ), this );

    auto inputField = new CheckingSpinBox<T>( this );
    inputField->setValue( initial );
    connect( inputField, &CheckingSpinBox<T>::valueChanged, std::move( callback ) );
    inputField->setPredicate( predicate );

    if constexpr ( std::is_floating_point_v<T> ) { inputField->setDecimals( dec ); }

    if ( !tooltip.empty() ) {
        inputLabel->setToolTip(
            QString( "<qt>%1</qt>" ).arg( QString( tooltip.c_str() ).toHtmlEscaped() ) );
    }

    inputLayout->addWidget( inputLabel );
    // inputLayout->addStretch();
    inputLayout->addWidget( inputField );

    m_currentLayout->addLayout( inputLayout );
}

template <typename T>
void ControlPanel::addNumberInput( const std::string& name,
                                   std::function<void( T )> callback,
                                   T initial,
                                   T min,
                                   T max,
                                   const std::string& tooltip,
                                   int dec ) {
    auto inputLayout = new QHBoxLayout();

    auto inputLabel = new QLabel( tr( name.c_str() ), this );

    QAbstractSpinBox* widget;
    if constexpr ( std::is_floating_point_v<T> ) {
        auto inputField = new QDoubleSpinBox( this );
        inputField->setDecimals( dec );
        inputField->setMinimum( min );
        inputField->setMaximum( max );
        inputField->setValue( initial );
        connect( inputField,
                 QOverload<double>::of( &QDoubleSpinBox::valueChanged ),
                 std::move( callback ) );
        widget = inputField;
    }
    else {
        auto inputField = new QSpinBox( this );
        inputField->setMinimum( min );
        inputField->setMaximum( max );
        inputField->setValue( initial );
        connect( inputField, &QSpinBox::valueChanged, std::move( callback ) );
        widget = inputField;
    }

    if ( !tooltip.empty() ) {
        inputLabel->setToolTip(
            QString( "<qt>%1</qt>" ).arg( QString( tooltip.c_str() ).toHtmlEscaped() ) );
    }
    inputLayout->addWidget( inputLabel );
    // inputLayout->addStretch();
    inputLayout->addWidget( widget );
    m_currentLayout->addLayout( inputLayout );
}

} // namespace Widgets
} // namespace Gui
} // namespace Ra
