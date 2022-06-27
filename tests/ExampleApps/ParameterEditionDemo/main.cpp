// Include Radium base application and its simple Gui
#include <Engine/Data/RenderParameters.hpp>
#include <Gui/MaterialParameterEditor/MaterialParameterEditor.hpp>

#include <nlohmann/json.hpp>

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>

using namespace Ra::Engine::Data;
using namespace Ra::Gui;

int main( int argc, char* argv[] )

{
    //! [Filling json parameter descriptor]
    auto parameterSet_metadata = R"(
    {
    "boolean_parameter": {
        "description": "Option/boolean value",
        "editable": true,
        "name": "Boolean",
        "type": "boolean"
        },
    "int_parameter": {
      "name": "Integer ",
      "type": "number"
    },
    "int_constrained_parameter": {
      "name": "Integer with constraint",
      "minimum": -10.0,
      "maximum": 10.0,
      "type": "number"
    },
    "uint_parameter": {
      "name": "Unsigned integer ",
      "type": "number"
    },
    "uint_constrained_parameter": {
      "name": "Integer with constraint",
      "minimum": 2.0,
      "maximum": 10.0,
      "type": "number"
    }
    }
    )"_json;

    //! [Creating the application]
    QApplication a( argc, argv );
    //! [Creating the application]

    QDialog dialog( nullptr );
    auto layout = new QVBoxLayout( dialog.window() );
    MaterialParameterEditor editor( dialog.window() );
    layout->addWidget( &editor );

    auto printParameter = []( const std::string& p ) {
        std::cout << "Parameter " << p << " was modified.\n";
    };

    QObject::connect(
        &editor, &MaterialParameterEditor::materialParametersModified, printParameter );

    RenderParameters parameters;

    parameters.addParameter( "boolean_parameter", false );
    parameters.addParameter( "int_parameter", int( 0 ) );
    parameters.addParameter( "int_constrained_parameter", int( 0 ) );
    parameters.addParameter( "uint_parameter", (unsigned int)( 0 ) );

    editor.setupFromParameters( parameters, parameterSet_metadata, "Example of parameters" );

    dialog.show();

    return a.exec();
}
