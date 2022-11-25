#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <Gui/Widgets/ControlPanel.hpp>

// include the Material Definition
#include <Engine/Data/Material.hpp>

#include <QString>
#include <QWidget>

#include <limits>
#include <memory>

using json = nlohmann::json;

namespace Ra {
using namespace Engine;
namespace Gui {

namespace internal {
class RenderParameterUiBuilder
{
  public:
    using types = Engine::Data::RenderParameters::BindableTypes;

    RenderParameterUiBuilder( ParameterSetEditor* pse, const json& constraints ) :
        m_pse { pse }, m_constraints { constraints } {}

    void operator()( const std::string& name,
                     const Data::RenderParameters::BoolParameter& p,
                     Data::RenderParameters&& params ) {
        auto onBoolParameterChanged = [pse = this->m_pse, &params, nm = name]( bool val ) {
            params.addParameter( nm, val );
            emit pse->parameterModified( nm );
        };
        if ( m_constraints.contains( name ) ) {
            if ( m_constraints[name]["editable"] ) {
                const auto& m           = m_constraints[name];
                std::string description = m.contains( "description" ) ? m["description"] : "";
                std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : name;
                m_pse->addOption( nm, onBoolParameterChanged, p.m_value, description );
            }
        }
        else if ( m_pse->m_showUnspecified ) {
            m_pse->addOption( name, onBoolParameterChanged, p.m_value );
        }
    }

    template <template <typename> typename TParam,
              typename TValue,
              std::enable_if_t<std::is_arithmetic<TValue>::value, bool> = true>
    void operator()( const std::string& name,
                     const TParam<TValue>& p,
                     Data::RenderParameters&& params ) {
        if ( params.getEnumConverter( name ) /*m_constraints.contains( name ) && m_constraints[name]["type"] == "enum"*/ ) {
            m_pse->addEnumParameterWidget( name, p.m_value, params, m_constraints );
        }
        else {
            // case number
            m_pse->addNumberParameterWidget( name, p.m_value, params, m_constraints );
        }
    }

    template <template <typename> typename TParam,
              typename TValue,
              std::enable_if_t<std::is_arithmetic<TValue>::value, bool> = true>
    void operator()( const std::string& name,
                     const TParam<std::vector<TValue>>& p,
                     Data::RenderParameters&& params ) {
        m_pse->addVectorParameterWidget( name, p.m_value, params, m_constraints );
    }

    void operator()( const std::string& name,
                     const Data::RenderParameters::ColorParameter& p,
                     Data::RenderParameters&& params ) {
        auto onColorParameterChanged =
            [pse = this->m_pse, &params, nm = name]( const Ra::Core::Utils::Color& val ) {
                params.addParameter( nm, val );
                emit pse->parameterModified( nm );
            };
        if ( m_constraints.contains( name ) ) {
            const auto& m           = m_constraints[name];
            std::string description = m.contains( "description" ) ? m["description"] : "";
            std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : name;
            m_pse->addColorInput(
                nm, onColorParameterChanged, p.m_value, m["maxItems"] == 4, description );
        }
        else if ( m_pse->m_showUnspecified ) {
            m_pse->addColorInput( name, onColorParameterChanged, p.m_value );
        }
    }

    template <template <typename> typename TParam,
              template <typename, int...>
              typename M,
              typename T,
              int... dim>
    void operator()( const std::string& name,
                     const TParam<M<T, dim...>>& p,
                     Data::RenderParameters&& params ) {
        m_pse->addMatrixParameterWidget( name, p.m_value, params, m_constraints );
    }

    void operator()( const std::string& /*name*/,
                     const Data::RenderParameters::TextureParameter& /*p*/,
                     Data::RenderParameters&& /*params*/ ) {
        // textures are not yet editable
    }

    void operator()( const std::string& /*name*/,
                     const std::reference_wrapper<Data::RenderParameters>& /*p*/,
                     Data::RenderParameters&& /*params*/ ) {
        // embeded render parameter edition not yet available
    }

  private:
    ParameterSetEditor* m_pse { nullptr };
    const json& m_constraints;
};
} // namespace internal

ParameterSetEditor::ParameterSetEditor( const std::string& name, QWidget* parent ) :
    ControlPanel( name, !name.empty(), parent ) {}

template <typename T>
void ParameterSetEditor::addEnumParameterWidget( const std::string& key,
                                                 T initial,
                                                 Ra::Engine::Data::RenderParameters& params,
                                                 const json& metadata ) {
    auto m = metadata[key];

    std::string description = m.contains( "description" ) ? m["description"] : "";
    std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : key;
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
        std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : key;

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
            else {
                addSliderInput( nm, onNumberParameterChanged, initial, min, max, description );
            }
        }
        else {
            min = m.contains( "minimum" ) ? T( m["minimum"] ) : min;
            max = m.contains( "maximum" ) ? T( m["maximum"] ) : max;
            addNumberInput<T>( nm, onNumberParameterChanged, initial, min, max, description );
        }
    }
    else if ( m_showUnspecified ) {
        addNumberInput<T>( key, onNumberParameterChanged, initial );
    }
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
    else if ( m_showUnspecified ) {
        addVectorInput<T>( key, onVectorParameterChanged, initial );
    }
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
    else if ( m_showUnspecified ) {
        addMatrixInput( key, onMatrixParameterChanged, initial );
    }
}

void ParameterSetEditor::setupFromParameters( Engine::Data::RenderParameters& params,
                                              const nlohmann::json& constraints ) {

    internal::RenderParameterUiBuilder uiBuilder { this, constraints };
    params.visit( uiBuilder, params );
    addStretch( 0 );
    setVisible( true );
}

void ParameterSetEditor::showUnspecified( bool enable ) {
    m_showUnspecified = enable;
}
} // namespace Gui
} // namespace Ra
