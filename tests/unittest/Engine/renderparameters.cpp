#include <catch2/catch.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/RadiumEngine.hpp>

using namespace Ra::Engine::Data;
using namespace Ra::Core;
using namespace Ra::Core::Utils;

#include <Core/Utils/TypesUtils.hpp>
class PrintThemAllVisitor : public VariableSet::DynamicVisitor
{
  public:
    template <typename T>
    void operator()( const std::string& name, const T& _in, std::any&& ) {
        std::cout << "\tPrintThemAllVisitor : ( " << Utils::demangleType<T>() << " ) " << name
                  << " --> " << _in << "\n";
    }

    template <typename T>
    void allowVisit( /* T = T {}*/ ) {
        addOperator<T>( *this );
    }
};

class StaticPrintVisitor
{
  public:
    using customTypes = Utils::TypeList<std::string>;
    // append custom types to the list of default BindableTypes
    using types = RenderParameters::BindableTypes::Append<customTypes>;

    StaticPrintVisitor() {
        std::cout << "StaticPrintVisitor will iterate on " << RenderParameters::BindableTypes::Size
                  << "+" << customTypes::Size << " types.\n";
    }

    template <typename T, typename std::enable_if<!std::is_class<T>::value, bool>::type = true>
    void operator()( const std::string& name, const T& _in, const std::string& prefix = "" ) {
        if ( !prefix.empty() ) { std::cout << "\t" << prefix << " : ( "; }
        else { std::cout << "\tStaticPrintVisitor : "; }
        std::cout << " (" << Utils::demangleType<T>() << " ) " << name << " --> " << _in << "\n";
    }

    template <typename T, typename std::enable_if<std::is_class<T>::value, bool>::type = true>
    void operator()( const std::string& name,
                     [[maybe_unused]] const T& _in,
                     const std::string& prefix = "" ) {
        if ( !prefix.empty() ) { std::cout << "\t" << prefix << " : ( "; }
        else { std::cout << "\tStaticPrintVisitor : "; }
        if constexpr ( std::is_same<T, std::string>::value ) {
            std::cout << " (" << Utils::demangleType<T>() << " ) " << name << " --> " << _in
                      << "\n";
        }
        else { std::cout << " (" << Utils::demangleType<T>() << " ) " << name << "\n"; }
    }

    void operator()( const std::string& name,
                     const std::reference_wrapper<RenderParameters>& p,
                     const std::string& prefix = "" ) {
        std::string localPrefix;
        if ( prefix.empty() ) { localPrefix = "StaticPrintVisitor : "; }
        else { localPrefix = prefix; }

        std::cout << "\t" << localPrefix << " (" << Utils::demangleType( p.get() ) << " ) " << name
                  << " --> visiting recursively\n";
        // visit the sub-parameters
        p.get().visit( *this, std::string { "\t" } + localPrefix );

        std::cout << "\t" << localPrefix << " (" << Utils::demangleType( p.get() ) << " ) " << name
                  << " --> end recursive visit\n";
    }
};

TEST_CASE( "Engine/Data/RenderParameters", "[Engine][Engine/Data][RenderParameters]" ) {
    using RP = RenderParameters;
    SECTION( "Parameter storage" ) {
        RP p1;
        REQUIRE( !( p1.hasParameterSet<int>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<bool>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<uint>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Scalar>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<std::vector<int>>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<std::vector<uint>>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<std::vector<Scalar>>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Vector2>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Vector3>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Vector4>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Utils::Color>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Matrix2>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Matrix3>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<Ra::Core::Matrix4>().has_value() ) );
        REQUIRE( !( p1.hasParameterSet<RP::TextureInfo>().has_value() ) );
        int i    = 1;
        uint ui  = 1u;
        Scalar s = 1_ra;
        bool b   = true;
        std::vector is { 0, 1, 2, 3, 4 };
        std::vector uis { 0u, 1u, 2u, 3u, 4u };
        std::vector ss { 0_ra, 1_ra, 2_ra, 3_ra, 4_ra };
        Vector2 vec2 { 0_ra, 1_ra };
        Vector3 vec3 { 0_ra, 1_ra, 2_ra };
        Vector4 vec4 { 0_ra, 1_ra, 2_ra, 4_ra };
        Matrix2 mat2 = Matrix2::Identity();
        Matrix3 mat3 = Matrix3::Identity();
        Matrix4 mat4 = Matrix4::Identity();
        Color color  = Color::White();
        Texture tex1 { { "texture1" } };

        p1.setTexture( "TextureParameter", &tex1, 1 );
        p1.setVariable( "IntParameter", i );
        p1.setVariable( "BoolParameter", b );
        p1.setVariable( "UIntParameter", ui );
        p1.setVariable( "ScalarParameter", s );
        p1.setVariable( "IntsParameter", is );
        p1.setVariable( "UIntsParameter", uis );
        p1.setVariable( "ScalarsParameter", ss );
        p1.setVariable( "Vec2Parameter", vec2 );
        p1.setVariable( "Vec3Parameter", vec3 );
        p1.setVariable( "Vec4Parameter", vec4 );
        p1.setVariable( "ColorParameter", color );
        p1.setVariable( "Mat2Parameter", mat2 );
        p1.setVariable( "Mat3Parameter", mat3 );
        p1.setVariable( "Mat4Parameter", mat4 );

        REQUIRE( p1.getParameterSet<int>().size() == 1 );
        REQUIRE( p1.getParameterSet<bool>().size() == 1 );
        REQUIRE( p1.getParameterSet<uint>().size() == 1 );
        REQUIRE( p1.getParameterSet<Scalar>().size() == 1 );
        REQUIRE( p1.getParameterSet<std::vector<int>>().size() == 1 );
        REQUIRE( p1.getParameterSet<std::vector<uint>>().size() == 1 );
        REQUIRE( p1.getParameterSet<std::vector<Scalar>>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector2>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector3>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector4>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Utils::Color>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix2>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix3>().size() == 1 );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix4>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::TextureInfo>().size() == 1 );

        REQUIRE( p1.getParameterSet<int>().at( "IntParameter" ) == i );
        REQUIRE( p1.getParameterSet<bool>().at( "BoolParameter" ) == b );
        REQUIRE( p1.getParameterSet<uint>().at( "UIntParameter" ) == ui );
        REQUIRE( p1.getParameterSet<Scalar>().at( "ScalarParameter" ) == s );
        REQUIRE( p1.getParameterSet<std::vector<int>>().at( "IntsParameter" ) == is );
        REQUIRE( p1.getParameterSet<std::vector<uint>>().at( "UIntsParameter" ) == uis );
        REQUIRE( p1.getParameterSet<std::vector<Scalar>>().at( "ScalarsParameter" ) == ss );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector2>().at( "Vec2Parameter" ) == vec2 );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector3>().at( "Vec3Parameter" ) == vec3 );
        REQUIRE( p1.getParameterSet<Ra::Core::Vector4>().at( "Vec4Parameter" ) == vec4 );
        REQUIRE( p1.getParameterSet<Ra::Core::Utils::Color>().at( "ColorParameter" ) == color );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix2>().at( "Mat2Parameter" ) == mat2 );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix3>().at( "Mat3Parameter" ) == mat3 );
        REQUIRE( p1.getParameterSet<Ra::Core::Matrix4>().at( "Mat4Parameter" ) == mat4 );
        REQUIRE( p1.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ).first == &tex1 );
        REQUIRE( p1.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ).second == 1 );

        StaticPrintVisitor vstr;
        p1.visit( vstr, "p1 parameter set" );

        RP p2;
        p2.setVariable( "IntParameter", i + 1 );
        p2.setVariable( "BoolParameter", !b );
        p2.setVariable( "UIntParameter", ui + 1 );
        p2.setVariable( "ScalarParameter", s + 1_ra );
        is.push_back( 0 );
        p2.setVariable( "IntsParameter", is );
        uis.push_back( 0u );
        p2.setVariable( "UIntsParameter", uis );
        ss.push_back( 0_ra );
        p2.setVariable( "ScalarsParameter", ss );
        p2.setVariable( "Vec2Parameter", Vector2 { vec2 + Vector2 { 1_ra, 1_ra } } );
        p2.setVariable( "Vec3Parameter", Vector3 { vec3 + Vector3 { 1_ra, 1_ra, 1_ra } } );
        p2.setVariable( "Vec4Parameter", Vector4 { vec4 + Vector4 { 1_ra, 1_ra, 1_ra, 1_ra } } );
        p2.setVariable( "ColorParameter", Color::Red() );
        Texture tex2 { { "texture2" } };
        p2.setTexture( "TextureParameter", &tex2, 2 );
        p2.setVariable( "Foo", 42 );

        // add a int parameter to p1
        p1.setVariable( "Bar", 43 );

        RP kept = p1;
        kept.mergeKeepVariables( p2 );

        // existings parameters are note changes (p1's values)
        REQUIRE( kept.getParameterSet<int>().at( "IntParameter" ) ==
                 p1.getParameterSet<int>().at( "IntParameter" ) );
        REQUIRE( kept.getParameterSet<bool>().at( "BoolParameter" ) ==
                 p1.getParameterSet<bool>().at( "BoolParameter" ) );
        REQUIRE( kept.getParameterSet<uint>().at( "UIntParameter" ) ==
                 p1.getParameterSet<uint>().at( "UIntParameter" ) );
        REQUIRE( kept.getParameterSet<Scalar>().at( "ScalarParameter" ) ==
                 p1.getParameterSet<Scalar>().at( "ScalarParameter" ) );
        REQUIRE( kept.getParameterSet<std::vector<int>>().at( "IntsParameter" ) ==
                 p1.getParameterSet<std::vector<int>>().at( "IntsParameter" ) );
        REQUIRE( kept.getParameterSet<std::vector<uint>>().at( "UIntsParameter" ) ==
                 p1.getParameterSet<std::vector<uint>>().at( "UIntsParameter" ) );
        REQUIRE( kept.getParameterSet<std::vector<Scalar>>().at( "ScalarsParameter" ) ==
                 p1.getParameterSet<std::vector<Scalar>>().at( "ScalarsParameter" ) );
        REQUIRE( kept.getParameterSet<Ra::Core::Vector2>().at( "Vec2Parameter" ) ==
                 p1.getParameterSet<Ra::Core::Vector2>().at( "Vec2Parameter" ) );
        REQUIRE( kept.getParameterSet<Ra::Core::Vector3>().at( "Vec3Parameter" ) ==
                 p1.getParameterSet<Ra::Core::Vector3>().at( "Vec3Parameter" ) );
        REQUIRE( kept.getParameterSet<Ra::Core::Vector4>().at( "Vec4Parameter" ) ==
                 p1.getParameterSet<Ra::Core::Vector4>().at( "Vec4Parameter" ) );
        REQUIRE( kept.getParameterSet<Ra::Core::Utils::Color>().at( "ColorParameter" ) ==
                 p1.getParameterSet<Ra::Core::Utils::Color>().at( "ColorParameter" ) );
        REQUIRE( kept.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ) ==
                 p1.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ) );
        // Foo is p2's value
        REQUIRE( kept.getParameterSet<int>().at( "Foo" ) == p2.getParameterSet<int>().at( "Foo" ) );

        // Bar is on p1 side only, still here
        REQUIRE( kept.getParameterSet<int>().at( "Bar" ) == p1.getParameterSet<int>().at( "Bar" ) );

        RP replaced = p1;
        replaced.mergeReplaceVariables( p2 );
        // Existings in p1 and p2, as well as new parameters are set to p2's values
        REQUIRE( replaced.getParameterSet<int>().at( "IntParameter" ) ==
                 p2.getParameterSet<int>().at( "IntParameter" ) );
        REQUIRE( replaced.getParameterSet<bool>().at( "BoolParameter" ) ==
                 p2.getParameterSet<bool>().at( "BoolParameter" ) );
        REQUIRE( replaced.getParameterSet<uint>().at( "UIntParameter" ) ==
                 p2.getParameterSet<uint>().at( "UIntParameter" ) );
        REQUIRE( replaced.getParameterSet<Scalar>().at( "ScalarParameter" ) ==
                 p2.getParameterSet<Scalar>().at( "ScalarParameter" ) );
        REQUIRE( replaced.getParameterSet<std::vector<int>>().at( "IntsParameter" ) ==
                 p2.getParameterSet<std::vector<int>>().at( "IntsParameter" ) );
        REQUIRE( replaced.getParameterSet<std::vector<uint>>().at( "UIntsParameter" ) ==
                 p2.getParameterSet<std::vector<uint>>().at( "UIntsParameter" ) );
        REQUIRE( replaced.getParameterSet<std::vector<Scalar>>().at( "ScalarsParameter" ) ==
                 p2.getParameterSet<std::vector<Scalar>>().at( "ScalarsParameter" ) );
        REQUIRE( replaced.getParameterSet<Ra::Core::Vector2>().at( "Vec2Parameter" ) ==
                 p2.getParameterSet<Ra::Core::Vector2>().at( "Vec2Parameter" ) );
        REQUIRE( replaced.getParameterSet<Ra::Core::Vector3>().at( "Vec3Parameter" ) ==
                 p2.getParameterSet<Ra::Core::Vector3>().at( "Vec3Parameter" ) );
        REQUIRE( replaced.getParameterSet<Ra::Core::Vector4>().at( "Vec4Parameter" ) ==
                 p2.getParameterSet<Ra::Core::Vector4>().at( "Vec4Parameter" ) );
        REQUIRE( replaced.getParameterSet<Ra::Core::Utils::Color>().at( "ColorParameter" ) ==
                 p2.getParameterSet<Ra::Core::Utils::Color>().at( "ColorParameter" ) );
        REQUIRE( replaced.getParameterSet<int>().at( "Foo" ) ==
                 p2.getParameterSet<int>().at( "Foo" ) );
        REQUIRE( replaced.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ) ==
                 p2.getParameterSet<RP::TextureInfo>().at( "TextureParameter" ) );
        // Bar is on p1 side only and not changed
        REQUIRE( replaced.getParameterSet<int>().at( "Bar" ) ==
                 p1.getParameterSet<int>().at( "Bar" ) );

        auto removed = replaced.deleteVariable<int>( "Bar" );
        REQUIRE( removed == true );
        auto found = replaced.containsParameter<int>( "Bar" );
        REQUIRE( found.has_value() == false );
    }

    SECTION( "Enum parameter" ) {
        RP params;

        enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };
        using ValuesType = typename std::underlying_type_t<Values>;

        enum Unregistered : int { LOW = -1, MIDDDLE = 0, HIGH = 1 };
        using UnregisteredType = typename std::underlying_type_t<Unregistered>;

        auto valuesEnumConverter = std::make_shared<Ra::Core::Utils::EnumConverter<ValuesType>>(
            std::initializer_list<std::pair<ValuesType, std::string>> {
                { Values::VALUE_0, "VALUE_0" },
                { Values::VALUE_1, "VALUE_1" },
                { Values::VALUE_2, "VALUE_2" } } );

        // Enum converter must be added and fetched using the enum underlying type
        REQUIRE( !params.getEnumConverter<ValuesType>( "enum.semantic" ) );
        params.addEnumConverter( "enum.semantic", valuesEnumConverter );
        REQUIRE( params.getEnumConverter<ValuesType>( "enum.semantic" ) );
        REQUIRE( !params.getEnumConverter<ValuesType>( "enum.unknown" ) );
        REQUIRE( !params.getEnumConverter<UnregisteredType>( "enum.unknown" ) );

        // The string value of an enum value (with the enumeration type) can be fetched from
        // the parameters and is empty if the enum is not registered
        REQUIRE( params.getEnumString( "enum.semantic", Values::VALUE_0 ) == "VALUE_0" );
        REQUIRE( params.getEnumString( "enum.unknown", Unregistered::LOW ) == "" );

        // Adding the enum in the parameter set using its value
        params.setEnumVariable( "enum.semantic", Values::VALUE_0 );
        // checking its seen with its type (enum)
        auto& v = params.getParameter<Values>( "enum.semantic" );
        REQUIRE( v == Values::VALUE_0 );
        // The string value of an enum value (with the enumeration's underlying type) can also be
        // fetched from the parameters
        REQUIRE( params.getEnumString( "enum.semantic", v ) == "VALUE_0" );

        // changing the value trough setParameter and string representation
        params.setEnumVariable( "enum.semantic", "VALUE_2" );
        REQUIRE( v == Values::VALUE_2 );

        // unregistered enum could be added only using their value
        params.setEnumVariable( "enum.unknown", Unregistered::LOW );
        auto u = params.getParameter<Unregistered>( "enum.unknown" );
        REQUIRE( u == Unregistered::LOW );
        REQUIRE( params.getEnumString( "enum.unknown", u ) == "" );

        // Trying to add unregistered enums values trough string does not change the stored value
        params.setEnumVariable( "enum.unknown", "Unregistered::HIGH" );
        u = params.getParameter<Unregistered>( "enum.unknown" );
        REQUIRE( u == Unregistered::LOW );
    }

    SECTION( "Parameter visit" ) {
        RP paramsToVisit;
        enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };
        using ValuesType = typename std::underlying_type<Values>::type;

        auto vnc =
            new Ra::Core::Utils::EnumConverter<ValuesType>( { { Values::VALUE_0, "VALUE_0" },
                                                              { Values::VALUE_1, "VALUE_1" },
                                                              { Values::VALUE_2, "VALUE_2" } } );
        auto valuesEnumConverter =
            std::shared_ptr<Ra::Core::Utils::EnumConverter<ValuesType>>( vnc );
        paramsToVisit.addEnumConverter( "enum.semantic", valuesEnumConverter );
        paramsToVisit.setEnumVariable( "enum.semantic", "VALUE_0" );
        paramsToVisit.setVariable( "int.simple", int( 1 ) );

        PrintThemAllVisitor ptm;
        ptm.allowVisit<ValuesType>();
        ptm.allowVisit<int>();
        std::cout << "Visiting with custom dynamic visitor :\n";
        paramsToVisit.visit( ptm );

        StaticPrintVisitor vstr;
        std::cout << "Visiting with custom static visitor :\n";
        paramsToVisit.visit( vstr );

        std::cout << "Visiting with custom static visitor and hierarchical parameters:\n";
        RP subParams;
        subParams.setVariable( "sub.int", 3 );
        subParams.setEnumVariable( "sub.string", "SubString" );
        subParams.setEnumVariable( "enum.semantic", "VALUE_1" );
        paramsToVisit.setVariable( "SubParameter", subParams );
        paramsToVisit.visit( vstr, "Visiting with subparameters" );
    }
}
