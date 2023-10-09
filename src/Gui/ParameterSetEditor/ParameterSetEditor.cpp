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

    void operator()( const std::string& name, bool& p, Data::RenderParameters&& /* params */ ) {
        auto onBoolParameterChanged = [pse = this->m_pse, &p, nm = name]( bool val ) {
            p = val;
            emit pse->parameterModified( nm );
        };
        if ( m_constraints.contains( name ) ) {
            if ( m_constraints[name]["editable"] ) {
                const auto& m           = m_constraints[name];
                std::string description = m.contains( "description" ) ? m["description"] : "";
                std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : name;
                m_pse->addOption( nm, onBoolParameterChanged, p, description );
            }
        }
        else if ( m_pse->m_showUnspecified ) {
            m_pse->addOption( name, onBoolParameterChanged, p );
        }
    }

    template <typename TParam, std::enable_if_t<std::is_arithmetic<TParam>::value, bool> = true>
    void operator()( const std::string& name, TParam& p, Data::RenderParameters&& params ) {
        if ( params.getEnumConverter<TParam>( name ) ) {
            m_pse->addEnumParameterWidget( name, p, params, m_constraints );
        }
        else {
            // case number
            m_pse->addNumberParameterWidget( name, p, params, m_constraints );
        }
    }

    template <typename TParam,
              typename TAllocator,
              std::enable_if_t<std::is_arithmetic<TParam>::value, bool> = true>
    void operator()( const std::string& name,
                     std::vector<TParam, TAllocator>& p,
                     Data::RenderParameters&& params ) {
        m_pse->addVectorParameterWidget( name, p, params, m_constraints );
    }

    void operator()( const std::string& name,
                     Ra::Core::Utils::Color& p,
                     Data::RenderParameters&& params ) {
        auto onColorParameterChanged =
            [pse = this->m_pse, &params, nm = name]( const Ra::Core::Utils::Color& val ) {
                params.setVariable( nm, val );
                emit pse->parameterModified( nm );
            };
        if ( m_constraints.contains( name ) ) {
            const auto& m           = m_constraints[name];
            std::string description = m.contains( "description" ) ? m["description"] : "";
            std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : name;
            m_pse->addColorInput( nm, onColorParameterChanged, p, m["maxItems"] == 4, description );
        }
        else if ( m_pse->m_showUnspecified ) {
            m_pse->addColorInput( name, onColorParameterChanged, p );
        }
    }

    template <template <typename, int...> typename M, typename T, int... dim>
    void operator()( const std::string& name, M<T, dim...>& p, Data::RenderParameters&& params ) {
        m_pse->addMatrixParameterWidget( name, p, params, m_constraints );
    }

    void operator()( const std::string& /*name*/,
                     Data::RenderParameters::TextureInfo& /*p*/,
                     Data::RenderParameters&& /*params*/ ) {
        // textures are not yet editable
    }

    template <typename T>
    void operator()( const std::string& /*name*/,
                     std::reference_wrapper<T>& /*p*/,
                     Data::RenderParameters&& /*params*/ ) {
        // wrapped reference (e.g. embedded render parameter) edition not yet available
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
                                                 T& initial,
                                                 Ra::Engine::Data::RenderParameters& params,
                                                 const json& metadata ) {
    auto m = metadata[key];

    std::string description = m.contains( "description" ) ? m["description"] : "";
    std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : key;
    if ( auto ec = params.getEnumConverter<T>( key ) ) {
        auto items                        = ( *ec )->getEnumerators();
        auto onEnumParameterStringChanged = [this, &params, &key]( const QString& value ) {
            params.setVariable( key, value.toStdString() );
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
            params.setVariable( key, value );
            emit parameterModified( key );
        };
        addComboBox( nm, onEnumParameterIntChanged, initial, items, description );
    }
}

template <typename T>
void ParameterSetEditor::addNumberParameterWidget( const std::string& key,
                                                   T& initial,
                                                   Ra::Engine::Data::RenderParameters& /*params*/,
                                                   const json& metadata ) {

    auto onNumberParameterChanged = [this, &initial, &key]( T value ) {
        initial = value;
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
                                                   std::vector<T>& initial,
                                                   Ra::Engine::Data::RenderParameters& /*params*/,
                                                   const json& metadata ) {
    auto onVectorParameterChanged = [this, &initial, &key]( const std::vector<T>& value ) {
        initial = value;
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
                                                   T& initial,
                                                   Ra::Engine::Data::RenderParameters& /*params*/,
                                                   const json& metadata ) {
    auto onMatrixParameterChanged = [this, &initial, &key]( const Ra::Core::MatrixN& value ) {
        initial = T( value );
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
