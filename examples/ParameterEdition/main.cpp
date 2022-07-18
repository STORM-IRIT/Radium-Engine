// Include Radium base application and its simple Gui
#include <Engine/Data/RenderParameters.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <nlohmann/json.hpp>

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>

using namespace Ra::Engine::Data;
using namespace Ra::Gui;

/// \todo use magicEnum to manage enumerations, string, value, .... for edition ?
enum Values : int { VALUE_0, VALUE_1, VALUE_2 };

int main( int argc, char* argv[] )

{
    //! [Filling json parameter descriptor]
    auto parameterSet_metadata = R"(
    {
    "bool": {
        "description": "Option/boolean value",
        "editable": true,
        "type": "boolean"
        },
    "enum": {
        "name": "My Enum",
        "description": "unscoped enum, ranging from 0 to n",
        "type": "enum",
        "values": [
          "VALUE_0",
          "VALUE_1",
          "VALUE_2"
        ]
    },
    "int_constrained": {
      "description": "Integer between -10 and 10",
      "minimum": -10,
      "maximum": 10,
      "type": "number"
    },
    "uint_constrained": {
      "description": "Unsigned integer between 2 and 10",
      "minimum": 2,
      "maximum": 10,
      "type": "number"
    },
    "Scalar_constrained": {
      "description": "Scalar value between 0.0 and 1.0",
      "minimum": 0.0,
      "maximum": 1.0,
      "type": "number"
    },
    "Scalar_half_constrained": {
      "description": "Scalar value greater or equal to -2.0",
      "minimum": -2.0,
      "type": "number"
    },
    "Scalar_Multiconstrained": {
    "description": "Scalar value between -1.0 and 1.0 or greater than 5.0",
    "oneOf": [
        {
          "minimum": -1.0,
          "maximum": 1.0
        },
        {
          "minimum": 5.0
        }
      ],
      "type": "number"
    }
    }
    )"_json;
    //! [Filling json parameter set descriptor]

    //! [Creating the application]
    QApplication a( argc, argv );
    //! [Creating the application]

    //! [Creating the edition dialog]
    QDialog dialog( nullptr );
    dialog.setWindowTitle( "ParameterSet edition example" );
    auto layout = new QVBoxLayout( dialog.window() );
    ParameterSetEditor editor( "Demonstration parameter set", dialog.window() );

    editor.showUnspecified( true );
    layout->addWidget( &editor );
    //! [Creating the edition dialog]

    RenderParameters parameters;

    parameters.addParameter( "bool", false );
    parameters.addParameter( "enum", Values::VALUE_0 );
    parameters.addParameter( "int", int( 0 ) );
    parameters.addParameter( "int_constrained", int( 0 ) );
    parameters.addParameter( "uint", (unsigned int)( 10 ) );
    parameters.addParameter( "uint_constrained", (unsigned int)( 5 ) );
    parameters.addParameter( "Scalar", 0_ra );
    parameters.addParameter( "Scalar_constrained", 0.5_ra );
    parameters.addParameter( "Scalar_half_constrained", 0_ra );
    parameters.addParameter( "Scalar_Multiconstrained", 0.5_ra );
    parameters.addParameter( "Color", Ra::Core::Utils::Color::Magenta() );
    parameters.addParameter( "Vec2", Ra::Core::Vector2 { 1_ra, 0_ra } );
    parameters.addParameter( "Vec3", Ra::Core::Vector3 { 1_ra, 1_ra, 1_ra } );
    parameters.addParameter(
        "Matrix3",
        Ra::Core::Matrix3 { { 0_ra, 0_ra, 0_ra }, { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra } } );
    parameters.addParameter( "std::vector<int>", std::vector<int> { 0, 1, 2 } );

    editor.setupFromParameters( parameters, parameterSet_metadata );

    auto printParameter = []( const std::string& p ) {
        std::cout << "Parameter " << p << " was modified.\n";
    };

    QObject::connect( &editor, &ParameterSetEditor::parameterModified, printParameter );

    dialog.show();

    return a.exec();
}
