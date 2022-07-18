#include <Gui/Widgets/ControlPanel.hpp>
#include <QLabel>

#include <Gui/Widgets/ConstrainedNumericSpinBox.hpp>
#include <Gui/Widgets/VectorEditor.hpp>

namespace Ra {
namespace Gui {
namespace Widgets {

template <typename T>
void ControlPanel::addConstrainedNumberInput( const std::string& name,
                                              std::function<void( T )> callback,
                                              Scalar initial,
                                              std::function<bool( T )> predicate,
                                              const std::string& tooltip,
                                              int dec ) {
    auto inputLayout = new QHBoxLayout();
    auto inputLabel  = new QLabel( tr( name.c_str() ), this );

    auto inputField = new ConstrainedNumericSpinBox<T>( this );
    inputField->setValue( initial );
    inputField->setMinimum( std::numeric_limits<T>::lowest() );
    inputField->setMaximum( std::numeric_limits<T>::max() );
    connect( inputField,
             qOverload<typename QtSpinBox::getType<T>::SignalType>(
                 &ConstrainedNumericSpinBox<T>::valueChanged ),
             std::move( callback ) );
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

    using WidgetType = typename QtSpinBox::getType<T>::Type;
    auto inputField  = new WidgetType( this );
    // to prevent overflow
    inputField->setRange(
        min,
        std::min( T( std::numeric_limits<typename QtSpinBox::getType<T>::SignalType>::max() ),
                  max ) );
    inputField->setValue( initial );
    connect( inputField,
             qOverload<typename QtSpinBox::getType<T>::SignalType>( &WidgetType::valueChanged ),
             std::move( callback ) );
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
void ControlPanel::addVectorInput( const std::string& name,
                                   std::function<void( const std::vector<T>& )> callback,
                                   const std::vector<T>& initial,
                                   const std::string& tooltip ) {
    auto inputLayout = new QHBoxLayout();

    auto inputLabel = new QLabel( tr( name.c_str() ), this );
    auto inputField = new VectorEditor<T>( initial, this );

    if ( !tooltip.empty() ) {
        inputLabel->setToolTip(
            QString( "<qt>%1</qt>" ).arg( QString( tooltip.c_str() ).toHtmlEscaped() ) );
    }
    inputLayout->addWidget( inputLabel );
    inputLayout->addWidget( inputField );
    connect( inputField,
             QOverload<const std::vector<T>&>::of( &VectorEditorSignals::valueChanged ),
             std::move( callback ) );
    m_currentLayout->addLayout( inputLayout );
}

} // namespace Widgets
} // namespace Gui
} // namespace Ra
