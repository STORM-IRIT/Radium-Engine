#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <Gui/Widgets/ControlPanel.hpp>

// include the Material Definition
#include <Engine/Data/Material.hpp>

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QString>
#include <QWidget>

#include <limits>
#include <memory>

namespace Ra::Gui {

ParameterSetEditor::ParameterSetEditor( const std::string& name, QWidget* parent ) :
    ControlPanel( name, !name.empty(), parent ) {}

using json = nlohmann::json;

template <typename T>
void ParameterSetEditor::addEnumParameterWidget( const std::string& key,
                                                 T initial,
                                                 Ra::Engine::Data::RenderParameters& params,
                                                 const json& metadata ) {
    auto m = metadata[key];

    std::string description = m.contains( "description" ) ? m["description"] : "";
    std::string nm          = m.contains( "name" ) ? m["name"] : key.c_str();
    if ( auto ec = params.getEnumConverter( key ) ) {
        auto items                        = ( *ec )->getEnumerators();
        auto onEnumParameterStringChanged = [this, &params, &key]( const QString& value ) {
            params.addParameter( key, value.toStdString() );
            emit parameterModified( key );
        };
        addComboBox( nm,
                     onEnumParameterStringChanged,
                     params.getEnumString( key, initial ),
                     items,
                     description );
    }
    else {
        LOG( Core::Utils::logWARNING )
            << "ParameterSet don't have converter for enum " << key << " use index<>int instead.";

        auto items = std::vector<std::string>();
        items.reserve( m["values"].size() );
        for ( const auto& value : m["values"] ) {
            items.push_back( value );
        }

        auto onEnumParameterIntChanged = [this, &params, &key]( T value ) {
            params.addParameter( key, value );
            emit parameterModified( key );
        };
        addComboBox( nm, onEnumParameterIntChanged, initial, items, description );
    }
}

template <typename T>
void ParameterSetEditor::addNumberParameterWidget( const std::string& key,
                                                   T initial,
                                                   Ra::Engine::Data::RenderParameters& params,
                                                   const json& metadata ) {

    auto onNumberParameterChanged = [this, &params, &key]( T value ) {
        params.addParameter( key, value );
        emit parameterModified( key );
    };
    if ( metadata.contains( key ) ) {
        auto m   = metadata[key];
        auto min = std::numeric_limits<T>::lowest();
        auto max = std::numeric_limits<T>::max();

        std::string description = m.contains( "description" ) ? m["description"] : "";
        std::string nm          = m.contains( "name" ) ? m["name"] : key.c_str();

        if ( m.contains( "oneOf" ) ) {
            // the variable has multiple valid bounds
            std::vector<std::pair<T, T>> bounds;
            bounds.reserve( m["oneOf"].size() );
            for ( const auto& bound : m["oneOf"] ) {
                auto mini = bound.contains( "minimum" ) ? T( bound["minimum"] ) : min;
                auto maxi = bound.contains( "maximum" ) ? T( bound["maximum"] ) : max;
                bounds.emplace_back( mini, maxi );
            }
            auto predicate = [bounds]( T value ) {
                bool valid = false;
                auto it    = bounds.begin();
                while ( !valid && it != bounds.end() ) {
                    valid = value >= ( *it ).first && value <= ( *it ).second;
                    ++it;
                }
                return valid;
            };

            addConstrainedNumberInput<T>(
                nm, onNumberParameterChanged, initial, predicate, description );
        }
        else if ( m.contains( "minimum" ) && m.contains( "maximum" ) ) {
            min = T( m["minimum"] );
            max = T( m["maximum"] );
            if constexpr ( std::is_floating_point_v<T> ) {
                addPowerSliderInput( nm, onNumberParameterChanged, initial, min, max, description );
            }
            else { addSliderInput( nm, onNumberParameterChanged, initial, min, max, description ); }
        }
        else {
            min = m.contains( "minimum" ) ? T( m["minimum"] ) : min;
            max = m.contains( "maximum" ) ? T( m["maximum"] ) : max;
            addNumberInput<T>( nm, onNumberParameterChanged, initial, min, max, description );
        }
    }
    else if ( m_showUnspecified ) { addNumberInput<T>( key, onNumberParameterChanged, initial ); }
}

template <typename T>
void ParameterSetEditor::addVectorParameterWidget( const std::string& key,
                                                   const std::vector<T>& initial,
                                                   Ra::Engine::Data::RenderParameters& params,
                                                   const json& metadata ) {
    auto onVectorParameterChanged = [this, &params, &key]( const std::vector<T>& value ) {
        params.addParameter( key, value );
        emit parameterModified( key );
    };

    if ( metadata.contains( key ) ) {
        auto m                  = metadata[key];
        std::string description = m.contains( "description" ) ? m["description"] : "";
        addVectorInput<T>( m["name"], onVectorParameterChanged, initial, description );
    }
    else if ( m_showUnspecified ) { addVectorInput<T>( key, onVectorParameterChanged, initial ); }
}

template <typename T>
void ParameterSetEditor::addMatrixParameterWidget( const std::string& key,
                                                   const T& initial,
                                                   Ra::Engine::Data::RenderParameters& params,
                                                   const json& metadata ) {
    auto onMatrixParameterChanged = [this, &params, &key]( const Ra::Core::MatrixN& value ) {
        auto v = T( value );
        params.addParameter( key, v );
        emit parameterModified( key );
    };

    if ( metadata.contains( key ) ) {
        auto m                  = metadata[key];
        std::string description = m.contains( "description" ) ? m["description"] : "";
        addMatrixInput( m["name"], onMatrixParameterChanged, initial, 3, description );
    }
    else if ( m_showUnspecified ) { addMatrixInput( key, onMatrixParameterChanged, initial ); }
}

void ParameterSetEditor::setupFromParameters( Engine::Data::RenderParameters& params,
                                              const nlohmann::json& constraints ) {
    // Add widgets to edit bool parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::BoolParameter>() ) {
        auto onBoolParameterChanged = [this, &params, key = key]( bool val ) {
            params.addParameter( key, val );
            emit parameterModified( key );
        };
        if ( constraints.contains( key ) ) {
            if ( constraints[key]["editable"] ) {
                const auto& m           = constraints[key];
                std::string description = m.contains( "description" ) ? m["description"] : "";
                std::string nm          = m.contains( "name" ) ? m["name"] : key.c_str();
                addOption( nm, onBoolParameterChanged, value.m_value, description );
            }
        }
        else if ( m_showUnspecified ) { addOption( key, onBoolParameterChanged, value.m_value ); }
    }

    // Add widgets to edit int parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::IntParameter>() ) {
        if ( constraints.contains( key ) && constraints[key]["type"] == "enum" ) {
            addEnumParameterWidget( key, value.m_value, params, constraints );
        }
        else {
            // case number
            addNumberParameterWidget( key, value.m_value, params, constraints );
        }
    }

    // Add widgets to edit unsigned parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::UIntParameter>() ) {
        if ( constraints.contains( key ) && constraints[key]["type"] == "enum" ) {
            addEnumParameterWidget( key, value.m_value, params, constraints );
        }
        else {
            // case number
            addNumberParameterWidget( key, value.m_value, params, constraints );
        }
    }

    // Add widgets to edit scalar parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::ScalarParameter>() ) {
        addNumberParameterWidget( key, value.m_value, params, constraints );
    }

    // Add widgets to edit color parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::ColorParameter>() ) {
        auto onColorParameterChanged =
            [this, &params, key = key]( const Ra::Core::Utils::Color& val ) {
                params.addParameter( key, val );
                emit parameterModified( key );
            };
        if ( constraints.contains( key ) ) {
            const auto& m           = constraints[key];
            std::string description = m.contains( "description" ) ? m["description"] : "";
            std::string nm          = m.contains( "name" ) ? m["name"] : key.c_str();
            addColorInput(
                nm, onColorParameterChanged, value.m_value, m["maxItems"] == 4, description );
        }
        else if ( m_showUnspecified ) {
            addColorInput( key, onColorParameterChanged, value.m_value );
        }
    }

    // populate widgets for Ints parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::IntsParameter>() ) {
        addVectorParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for UInts parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::UIntsParameter>() ) {
        addVectorParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Scalars parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::ScalarsParameter>() ) {
        addVectorParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Vec2 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Vec2Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Vec3 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Vec3Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Vec4 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Vec4Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Mat2 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Mat2Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Mat3 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Mat3Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }

    // populate widgets for Mat4 parameters
    for ( auto& [key, value] :
          params.getParameterSet<Ra::Engine::Data::RenderParameters::Mat4Parameter>() ) {
        addMatrixParameterWidget( key, value.m_value, params, constraints );
    }
    addStretch( 0 );
    setVisible( true );
}

void ParameterSetEditor::showUnspecified( bool enable ) {
    m_showUnspecified = enable;
}
} // namespace Ra::Gui
