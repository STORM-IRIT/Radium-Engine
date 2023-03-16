// Include Radium base application and its simple Gui

#include <Core/Utils/TypesUtils.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <nlohmann/json.hpp>

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>

#include <typeinfo>

using namespace Ra::Engine::Data;
using namespace Ra::Gui;
using namespace Ra::Core;

/* Changed the underlying type to verify the good behavior of the edition */
enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };

// TODO : replace the following by a visitor. See ParameterSetEditor.
template <typename RP>
void printParameterValue( const RenderParameters& parameters, const ::std::string& p ) {
    if ( auto param = parameters.containsParameter<RP>( p ); param )
        std::cout << ( *param )->second << " (" << Utils::demangleType<RP>() << ")";
}

template <typename T, int N>
void printVectorParameterValue( const RenderParameters& parameters, const ::std::string& p ) {
    using RP = Eigen::Matrix<T, N, 1>;
    if ( auto param = parameters.containsParameter<RP>( p ); param )
        std::cout << ( *param )->second.transpose() << " (" << Utils::demangleType<RP>() << ")";
}

void printColorParameterValue( const RenderParameters& parameters, const ::std::string& p ) {
    using RP = RenderParameters::ColorParameter;
    if ( auto param = parameters.containsParameter<RP>( p ); param )
        std::cout << ( *param )->second.transpose() << " (" << Utils::demangleType<RP>() << ")";
}

template <typename T, int N, int M>
void printMatrixParameterValue( const RenderParameters& parameters, const ::std::string& p ) {
    using RP = Eigen::Matrix<T, N, M>;
    if ( auto param = parameters.containsParameter<RP>( p ); param )
        std::cout << "\n" << ( *param )->second << "\n (" << Utils::demangleType<RP>() << ")";
}

template <typename T>
void printCollectionParameterValue( const RenderParameters& parameters, const ::std::string& p ) {
    using RP = std::vector<T>;
    if ( auto param = parameters.containsParameter<RP>( p ); param ) {
        std::cout << "\n";
        const auto& v = ( *param )->second;
        std::copy( v.begin(), v.end(), std::ostream_iterator<T>( std::cout, " " ) );
        std::cout << "\n (" << Utils::demangleType<RP>() << ")";
    }
}

template <typename T>
void printAllParameters( const RenderParameters& parameters ) {
    auto& params = parameters.getParameterSet<T>();
    std::cout << "Parameters for type " << Utils::demangleType<T>() << " : " << params.size()
              << "\n";
    for ( const auto& [key, p] : params ) {
        std::cout << "\t" << key << " = " << p << "\n";
    }
}

int main( int argc, char* argv[] ) {
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
        "description": "unscoped enum, ranging from 10 to n30",
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

    //! [Management of string<->value conversion for enumeration parameters]
    auto vnc = new RenderParameters::EnumConverter<Values>( { { Values::VALUE_0, "VALUE_0" },
                                                              { Values::VALUE_1, "VALUE_1" },
                                                              { Values::VALUE_2, "VALUE_2" } } );
    auto valuesEnumConverter = std::shared_ptr<RenderParameters::EnumConverter<Values>>( vnc );
    //! [Management of string<->value conversion for enumeration parameters]

    //! [filling the parameter set to edit ]
    RenderParameters parameters;
    parameters.addEnumConverter( "enum", valuesEnumConverter );
    parameters.addParameter( "bool", false );
    parameters.addParameter( "enum", Values::VALUE_1 );
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
    //! [filling the parameter set to edit ]

    //! [Printing several parameters before edition ]
    std::cout << "\nPrinting all parameters before editiong :\n";
    printAllParameters<RenderParameters::IntParameter>( parameters );
    printAllParameters<RenderParameters::BoolParameter>( parameters );
    printAllParameters<RenderParameters::UIntParameter>( parameters );
    printAllParameters<RenderParameters::ScalarParameter>( parameters );
    //! [Printing several parameters before edition ]

    //! [Filling the editor with the parameter set ]
    editor.setupFromParameters( parameters, parameterSet_metadata );
    auto printParameter = [&parameters]( const std::string& p ) {
        std::cout << "Parameter " << p << " was modified. New value is ";
        printParameterValue<RenderParameters::IntParameter>( parameters, p );
        printParameterValue<RenderParameters::BoolParameter>( parameters, p );
        printParameterValue<RenderParameters::UIntParameter>( parameters, p );
        printParameterValue<RenderParameters::ScalarParameter>( parameters, p );
        printCollectionParameterValue<int>( parameters, p );
        printCollectionParameterValue<unsigned int>( parameters, p );
        printCollectionParameterValue<Scalar>( parameters, p );
        printVectorParameterValue<Scalar, 2>( parameters, p );
        printVectorParameterValue<Scalar, 3>( parameters, p );
        printVectorParameterValue<Scalar, 4>( parameters, p );
        printColorParameterValue( parameters, p );
        printMatrixParameterValue<Scalar, 2, 2>( parameters, p );
        printMatrixParameterValue<Scalar, 3, 3>( parameters, p );
        printMatrixParameterValue<Scalar, 4, 4>( parameters, p );
        std::cout << "\n";
    };
    QObject::connect( &editor, &ParameterSetEditor::parameterModified, printParameter );
    dialog.show();
    //! [Filling the editor with the parameter set ]

    a.exec();

    //! [Printing several parameters after edition ]
    std::cout << "\nPrinting all parameters before quit : ";
    printAllParameters<RenderParameters::IntParameter>( parameters );
    printAllParameters<RenderParameters::BoolParameter>( parameters );
    printAllParameters<RenderParameters::UIntParameter>( parameters );
    printAllParameters<RenderParameters::ScalarParameter>( parameters );
    //! [Printing several parameters after edition ]
}
