// Include Radium base application and its simple Gui

#include "Core/Containers/MakeShared.hpp"
#include <Core/Containers/VariableSet.hpp>
#include <Core/Containers/VariableSetEnumManagement.hpp>
#include <Core/Utils/TypesUtils.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Gui/ParameterSetEditor/BasicUiBuilder.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

#include <nlohmann/json.hpp>

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>

#include <type_traits>

using namespace Ra::Engine::Data;
using namespace Ra::Gui;
using namespace Ra::Core;

// Enumeration that will be available as string or numeric value
// four cases
// enum class with enum converter
enum class Values1 : unsigned int { VALUE1_0 = 10, VALUE1_1 = 20, VALUE1_2 = 30 };
using ValuesType1 = typename std::underlying_type_t<Values1>;

// classic enum, added as int in set -> int spin box
enum Values2 : unsigned int { VALUE2_0 = 1, VALUE2_1 = 2, VALUE2_2 = 3 };
using Values2Type2 = typename std::underlying_type_t<Values2>;

// classic enum, added as int in set with converter -> enum combo box
enum Values3 : unsigned int { VALUE3_0 = 100, VALUE3_1 = 200, VALUE3_2 = 300 };
using ValuesType3 = typename std::underlying_type_t<Values3>;

// enum class without converter, but with values in constraints -> enum combo box
enum class Values4 : unsigned int { VALUE4_0 = 0, VALUE4_1 = 1 };
using ValuesType4 = typename std::underlying_type_t<Values4>;

// enum class without converter nor values are ignored.

// Visitor to print all or some of the parameters stored in a RenderParameter object.
// This visitor is very similar to the one used to build the editing ui (RenderParameterUiBuilder
// in ParameterSetEditor.cpp) and could be used with a predicate function accepting variable wrt
// their name
struct ParameterPrinter {
    using types = Ra::Engine::Data::RenderParameters::BindableTypes::Append<Values1, Values4>;
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
            std::cout << name << " (" << Utils::demangleType<std::vector<T>>() << ") --> [";
            std::copy( value.begin(),
                       std::prev( value.end() ),
                       std::ostream_iterator<T>( std::cout, ", " ) );
            if ( !value.empty() ) { std::cout << value.back(); }
            std::cout << "]\n";
        }
    }

    template <typename T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    void operator()(
        const std::string& name,
        T& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<T>() << ") --> "
                      << static_cast<std::underlying_type_t<T>>( value ) << "\n";
        }
    }

    void operator()(
        const std::string& name,
        Ra::Core::Utils::Color& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<Ra::Core::Utils::Color>() << ") --> (";
            std::cout << value.transpose() << ")\n";
        }
    }

    template <template <typename, int...> typename M, typename T, int... dim>
    void operator()(
        const std::string& name,
        M<T, dim...>& value,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<M<T, dim...>>() << ") --> ";
            // transpose column vector for display
            if constexpr ( M<T, dim...>::ColsAtCompileTime == 1 )
                std::cout << "(" << value.transpose() << ")\n";
            else
                std::cout << "\n" << value << "\n";
        }
    }

    void operator()(
        const std::string& /*name*/,
        Ra::Engine::Data::RenderParameters::TextureInfo& /*p*/,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        // textures are not yet editable
    }

    template <typename T,
              std::enable_if_t<std::is_assignable_v<RenderParameters, typename std::decay<T>::type>,
                               bool> = true>
    void operator()(
        const std::string& name,
        T& p,
        SelectionPredicate&& pred = []( const std::string& ) { return true; } ) {
        if ( pred( name ) ) {
            std::cout << name << " (" << Utils::demangleType<T>() << ") --> \n";
            p.visit( *this, pred );
            std::cout << " <-- " << name << "\n";
        }
    }
};

int main( int argc, char* argv[] ) {
    //! [Filling json parameter descriptor]
    auto parameterSet_metadata = R"(
    {
    "bool": {
        "description": "Option/boolean value",
        "editable": true,
        "type": "boolean"
        },
    "enum1": {
        "description": "enum class with converter",
        "type": "enum"
    },
    "enum2": {
        "description": "plain enum without converter, added as uint",
        "type": "enum"
    },
    "enum3": {
        "description": "plain enum with converter, added as uint",
        "type": "enum"
    },
    "enum4": {
        "description": "enum class without converter, values are mendatory",
        "type": "enum",
        "values":["VALUE4_0","VALUE4_1"]
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
    "Scalar_multiconstrained": {
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

    //! [Creating the editing dialog]
    QDialog dialog( nullptr );
    dialog.setWindowTitle( "ParameterSet editing example" );
    auto layout = new QVBoxLayout( dialog.window() );
    VariableSetEditor editor( "Demonstration parameter set", dialog.window() );

    editor.setShowUnspecified( true );
    layout->addWidget( &editor );
    //! [Creating the editing dialog]

    //! [Management of string<->value conversion for enumeration parameters]
    using namespace Ra::Core::Utils;
    auto valuesEnumConverter1 = std::make_shared<EnumConverter<Values1>>(
        EnumConverter<Values1>::InitializerList { { Values1::VALUE1_0, "VALUE1_0" },
                                                  { Values1::VALUE1_1, "VALUE1_1" },
                                                  { Values1::VALUE1_2, "VALUE1_2" } } );

    auto valuesEnumConverter3 = std::make_shared<EnumConverter<ValuesType3>>(
        EnumConverter<ValuesType3>::InitializerList { { Values3::VALUE3_0, "VALUE3_0" },
                                                      { Values3::VALUE3_1, "VALUE3_1" },
                                                      { Values3::VALUE3_2, "VALUE3_2" } } );
    //! [Management of string<->value conversion for enumeration parameters]

    //! [filling the parameter set to edit ]
    RenderParameters parameters;
    using namespace Ra::Core::VariableSetEnumManagement;
    parameters.setVariable( "bool", false );
    addEnumConverter( parameters, "enum1", valuesEnumConverter1 );
    addEnumConverter( parameters, "enum3", valuesEnumConverter3 );
    parameters.setVariable( "enum1", Values1::VALUE1_1 );
    setEnumVariable( parameters, "enum2", Values2::VALUE2_1 );
    setEnumVariable( parameters, "enum3", Values3::VALUE3_2 );
    parameters.setVariable( "enum4", Values4::VALUE4_0 );
    parameters.setVariable( "int", int( 0 ) );
    parameters.setVariable( "int_constrained", int( 0 ) );
    parameters.setVariable( "uint", (unsigned int)( 10 ) );
    parameters.setVariable( "uint_constrained", (unsigned int)( 5 ) );

    float f;
    parameters.setVariable( "FLOAT REF", std::ref( f ) );
    parameters.setVariable( "Scalar", 0_ra );
    parameters.setVariable( "Scalar_constrained", 0.5_ra );
    parameters.setVariable( "Scalar_half_constrained", 0_ra );
    parameters.setVariable( "Scalar_multiconstrained", 0.5_ra );
    parameters.setVariable( "Color", Ra::Core::Utils::Color::Magenta() );
    parameters.setVariable( "Vec2", Ra::Core::Vector2 { 1_ra, 0_ra } );
    parameters.setVariable( "Vec3", Ra::Core::Vector3 { 1_ra, 1_ra, 1_ra } );
    parameters.setVariable(
        "Matrix3",
        Ra::Core::Matrix3 { { 0_ra, 0_ra, 0_ra }, { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra } } );
    parameters.setVariable( "std::vector<int>", std::vector<int> { 0, 1, 2 } );

    RenderParameters embedded;
    embedded.setVariable( "embedded.int value", 1 );
    embedded.setVariable( "embedded.scalar value", 1_ra );
    parameters.setVariable( "embedded", embedded );
    //! [filling the parameter set to edit ]

    //! [Printing several parameters before editing ]
    std::cout << "\nPrinting all parameters before editing :\n";
    parameters.visit( ParameterPrinter {} );
    //! [Printing several parameters before editing ]

    //! [Filling the editor with the parameter set ]

    BasicUiBuilder builder { parameters, &editor, parameterSet_metadata };
    // extends visited types, functor already present as template accepting
    // std::is_assignable_v<VariableSet>
    builder.addOperator<RenderParameters>( builder );

    // add operator for enum1, functor already present as template accepting std::is_enum<enum>
    builder.addOperator<Values1>( builder );
    builder.addOperator<Values2>( builder ); // do nothing, since enum2 added as int
    builder.addOperator<Values3>( builder ); // do nothing, since enum3 added as int
    builder.addOperator<Values4>( builder );

    parameters.visit( builder );

    auto printParameter = [&parameters]( const std::string& p ) {
        std::cout << "Parameter " << p << " was modified. New value is ";
        parameters.visit( ParameterPrinter {},
                          [p]( const std::string& name ) { return p == name; } );
        std::cout << "\n";
    };
    QObject::connect( &editor, &VariableSetEditor::parameterModified, printParameter );
    dialog.show();
    //! [Filling the editor with the parameter set ]

    a.exec();

    //! [Printing several parameters after editing ]
    std::cout << "\nPrinting all parameters before quit : ";
    parameters.visit( ParameterPrinter {} );
    //! [Printing several parameters after editing ]
}
