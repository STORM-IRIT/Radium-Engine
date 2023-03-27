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

// Visitor to print all or some of the parameters stored in a RenderParameter object.
// This visitor is very similar to the one used to build the edition ui (RenderParameterUiBuilder
// in ParameterSetEditor.cpp) and could be used with a predicate function accepting variable wrt
// their name
struct ParameterPrinter {
    using types              = Ra::Engine::Data::RenderParameters::BindableTypes;
    using SelectionPredicate = std::function<bool( const std::string& )>;

    void operator()(
        const std::string& name,
        bool& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<bool>() << ") --> " << std::boolalpha
                      << value << std::noboolalpha << "\n";
        }
    }

    template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
    void operator()(
        const std::string& name,
        T& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<T>() << ") --> " << value << "\n";
        }
    }

    template <typename T,
              typename TAllocator,
              std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
    void operator()(
        const std::string& name,
        std::vector<T, TAllocator>& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<T>() << ") --> [";
            std::copy( value.begin(),
                       std::prev( value.end() ),
                       std::ostream_iterator<T>( std::cout, ", " ) );
            if ( !value.empty() ) { std::cout << value.back(); }
            std::cout << "]\n";
        }
    }

    void operator()(
        const std::string& name,
        Ra::Core::Utils::Color& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<Ra::Core::Utils::Color>() << ") --> ";
            std::cout << value.transpose() << "\n";
        }
    }

    template <template <typename, int...> typename M, typename T, int... dim>
    void operator()(
        const std::string& name,
        M<T, dim...>& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<M<T, dim...>>() << ") --> \n"
                      << value << "\n";
        }
    }

    void operator()(
        const std::string& /*name*/,
        Ra::Engine::Data::RenderParameters::TextureInfo& /*p*/,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        // textures are not yet editable
    }

    void operator()(
        const std::string& /*name*/,
        std::reference_wrapper<Ra::Engine::Data::RenderParameters>& /*p*/,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        // embeded render parameter edition not yet available
    }
};

// Enumeration that will be available as string or numeric value
enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };
using ValuesType = typename std::underlying_type_t<Values>;

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
        "description": "unscoped enum, ranging from 10 to 30 with step 10",
        "type": "enum"
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
    auto vnc = new Ra::Core::Utils::EnumConverter<ValuesType>( { { Values::VALUE_0, "VALUE_0" },
                                                                 { Values::VALUE_1, "VALUE_1" },
                                                                 { Values::VALUE_2, "VALUE_2" } } );
    auto valuesEnumConverter = std::shared_ptr<Ra::Core::Utils::EnumConverter<ValuesType>>( vnc );
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
    parameters.addParameter( "Scalar_multiconstrained", 0.5_ra );
    parameters.addParameter( "Color", Ra::Core::Utils::Color::Magenta() );
    parameters.addParameter( "Vec2", Ra::Core::Vector2 { 1_ra, 0_ra } );
    parameters.addParameter( "Vec3", Ra::Core::Vector3 { 1_ra, 1_ra, 1_ra } );
    parameters.addParameter(
        "Matrix3",
        Ra::Core::Matrix3 { { 0_ra, 0_ra, 0_ra }, { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra } } );
    parameters.addParameter( "std::vector<int>", std::vector<int> { 0, 1, 2 } );
    //! [filling the parameter set to edit ]

    //! [Printing several parameters before edition ]
    std::cout << "\nPrinting all parameters before edition :\n";
    parameters.visit( ParameterPrinter {} );
    //! [Printing several parameters before edition ]

    //! [Filling the editor with the parameter set ]
    editor.setupFromParameters( parameters, parameterSet_metadata );
    auto printParameter = [&parameters]( const std::string& p ) {
        std::cout << "Parameter " << p << " was modified. New value is ";
        parameters.visit( ParameterPrinter {},
                          [p]( const std::string name ) { return p == name; } );
        std::cout << "\n";
    };
    QObject::connect( &editor, &ParameterSetEditor::parameterModified, printParameter );
    dialog.show();
    //! [Filling the editor with the parameter set ]

    a.exec();

    //! [Printing several parameters after edition ]
    std::cout << "\nPrinting all parameters before quit : ";
    parameters.visit( ParameterPrinter {} );

    //! [Printing several parameters after edition ]
}
