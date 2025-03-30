#pragma once

#include <Gui/RaGui.hpp>

#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <nlohmann/json.hpp>

#include <Core/Containers/DynamicVisitor.hpp>
#include <Core/Containers/VariableSet.hpp>
#include <Core/Containers/VariableSetEnumManagement.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>
#include <Gui/Widgets/ControlPanel.hpp>

namespace Ra {

namespace Gui {

class BasicUiBuilder : public Ra::Core::DynamicVisitor
{
  public:
    using VariableSet      = Ra::Core::VariableSet;
    using RenderParameters = Ra::Engine::Data::RenderParameters;

    BasicUiBuilder( VariableSet& params,
                    VariableSetEditor* pse,
                    const nlohmann::json& constraints ) :
        m_params { params }, m_pse { pse }, m_constraints { constraints } {
        addOperator<bool>( *this );
        addOperator<int>( *this );
        addOperator<unsigned int>( *this );
        addOperator<Scalar>( *this );
        addOperator<std::vector<int>>( *this );
        addOperator<Ra::Core::Utils::Color>( *this );
        addOperator<Ra::Core::Vector2>( *this );
        addOperator<Ra::Core::Vector3>( *this );
        addOperator<Ra::Core::Vector4>( *this );
        addOperator<Ra::Core::Matrix3>( *this );
        addOperator<Ra::Core::Matrix4>( *this );
        addOperator<std::reference_wrapper<VariableSet>>( *this );
        addOperator<std::reference_wrapper<const VariableSet>>( *this );
    }

    void operator()( const std::string& name, bool& p ) {
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
    void operator()( const std::string& name, TParam& p ) {
        using namespace Ra::Core::VariableSetEnumManagement;

        if ( getEnumConverter<TParam>( m_params, name ) ) {
            // known enum
            m_pse->addEnumWidget( name, p, m_params, m_constraints );
        }
        else {
            // number (or unknown enum
            m_pse->addNumberWidget( name, p, m_params, m_constraints );
        }
    }

    template <typename TParam,
              typename TAllocator,
              std::enable_if_t<std::is_arithmetic<TParam>::value, bool> = true>
    void operator()( const std::string& name, std::vector<TParam, TAllocator>& p ) {
        m_pse->addVectorWidget( name, p, m_params, m_constraints );
    }

    void operator()( const std::string& name, Ra::Core::Utils::Color& p ) {
        auto onColorParameterChanged = [pse = this->m_pse, &params = m_params, nm = name](
                                           const Ra::Core::Utils::Color& val ) {
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
    void operator()( const std::string& name, M<T, dim...>& p ) {
        m_pse->addMatrixWidget( name, p, m_params, m_constraints );
    }

    //    void operator()( const std::string& /*name*/,
    //                   Data::RenderParameters::TextureInfo& /*p*/,
    //               Core::VariableSet&& /*params*/ ) {
    // textures are not yet editable
    //}

    template <typename T>
    void operator()( const std::string& name, std::reference_wrapper<T>& p ) {
        m_pse->addLabel( name );
        if constexpr ( std::is_assignable_v<VariableSet, typename std::decay<T>::type> ) {
            if constexpr ( std::is_const_v<T> ) {
                p.get().visit(
                    *this, const_cast<VariableSet&>( static_cast<const VariableSet&>( p.get() ) ) );
            }
            else { p.get().visit( *this, static_cast<VariableSet&>( p.get() ) ); }
        }
    }

  private:
    VariableSet& m_params;
    VariableSetEditor* m_pse { nullptr };
    const nlohmann::json& m_constraints;
};

} // namespace Gui
} // namespace Ra
