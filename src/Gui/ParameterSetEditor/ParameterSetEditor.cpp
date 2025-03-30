#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <Engine/Data/Material.hpp>
#include <Gui/Widgets/ControlPanel.hpp>

#include <QString>
#include <QWidget>

#include <limits>
#include <memory>
#include <type_traits>

using json = nlohmann::json;

namespace Ra {
using namespace Engine;
namespace Gui {

namespace internal {
///\todo use a dynamic visitor with add operator, hence this is customizable
class RenderParameterUiBuilder
{
  public:
    using types = Engine::Data::RenderParameters::BindableTypes;

    RenderParameterUiBuilder( VariableSetEditor* pse, const json& constraints ) :
        m_pse { pse }, m_constraints { constraints } {}

    void operator()( const std::string& name, bool& p, Core::VariableSet&& /* params */ ) {
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
        else if ( m_pse->showUnspecified() ) {
            m_pse->addOption( name, onBoolParameterChanged, p );
        }
    }

    template <typename TParam, std::enable_if_t<std::is_arithmetic<TParam>::value, bool> = true>
    void operator()( const std::string& name, TParam& p, Core::VariableSet&& params ) {
        using namespace Ra::Core::VariableSetEnumManagement;
        if ( getEnumConverter<TParam>( params, name ) ) {
            m_pse->addEnumWidget( name, p, params, m_constraints );
        }
        else {
            // case number
            m_pse->addNumberWidget( name, p, params, m_constraints );
        }
    }

    template <typename TParam,
              typename TAllocator,
              std::enable_if_t<std::is_arithmetic<TParam>::value, bool> = true>
    void operator()( const std::string& name,
                     std::vector<TParam, TAllocator>& p,
                     Core::VariableSet&& params ) {
        m_pse->addVectorWidget( name, p, params, m_constraints );
    }

    void
    operator()( const std::string& name, Ra::Core::Utils::Color& p, Core::VariableSet&& params ) {
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
        else if ( m_pse->showUnspecified() ) {
            m_pse->addColorInput( name, onColorParameterChanged, p );
        }
    }

    template <template <typename, int...> typename M, typename T, int... dim>
    void operator()( const std::string& name, M<T, dim...>& p, Core::VariableSet&& params ) {
        m_pse->addMatrixWidget( name, p, params, m_constraints );
    }

    void operator()( const std::string& /*name*/,
                     Data::RenderParameters::TextureInfo& /*p*/,
                     Core::VariableSet&& /*params*/ ) {
        // textures are not yet editable
    }

    template <typename T>
    void operator()( const std::string& name,
                     std::reference_wrapper<T>& p,
                     Core::VariableSet&& /*params*/ ) {
        m_pse->addLabel( name );
        if constexpr ( std::is_assignable_v<Core::VariableSet, typename std::decay<T>::type> ) {
            if constexpr ( std::is_const_v<T> ) {
                p.get().visit( *this,
                               const_cast<Core::VariableSet&>(
                                   static_cast<const Core::VariableSet&>( p.get() ) ) );
            }
            else { p.get().visit( *this, static_cast<Core::VariableSet&>( p.get() ) ); }
        }
    }

  private:
    VariableSetEditor* m_pse { nullptr };
    const json& m_constraints;
};
} // namespace internal

VariableSetEditor::VariableSetEditor( const std::string& name, QWidget* parent ) :
    ControlPanel( name, !name.empty(), parent ) {}

void VariableSetEditor::setupUi( Core::VariableSet& params, const nlohmann::json& constraints ) {

    internal::RenderParameterUiBuilder uiBuilder { this, constraints };
    params.visit( uiBuilder, params );
    addStretch( 0 );
    setVisible( true );
}

} // namespace Gui
} // namespace Ra
