#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Gui/ParameterSetEditor/BasicUiBuilder.hpp>

#include <QLineEdit>
#include <string>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

class RA_DATAFLOW_GUI_API WidgetFactory : public Ra::Gui::BasicUiBuilder
{
  public:
    WidgetFactory( Ra::Core::VariableSet& params,
                   Ra::Gui::VariableSetEditor* pse,
                   const nlohmann::json& constraints ) :
        Ra::Gui::BasicUiBuilder( params, pse, constraints ) {
        addOperator<std::string>( *this );
    }

    void operator()( const std::string& name, std::string& p );
};

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
