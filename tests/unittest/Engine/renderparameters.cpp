#include <catch2/catch.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/RadiumEngine.hpp>

using namespace Ra::Engine::Data;
using namespace Ra::Core;
using namespace Ra::Core::Utils;

TEST_CASE( "Engine/Data/RenderParameters", "[Engine][Engine/Data][RenderParameters]" ) {
    using RP = RenderParameters;
    SECTION( "Parameter storage" ) {
        RP p1;
        REQUIRE( p1.getParameterSet<RP::IntParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::BoolParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::UIntParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::ScalarParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::IntsParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::UIntsParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::ScalarsParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Vec2Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Vec3Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Vec4Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::ColorParameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Mat2Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Mat3Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::Mat4Parameter>().size() == 0 );
        REQUIRE( p1.getParameterSet<RP::TextureParameter>().size() == 0 );
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

        p1.addParameter( "IntParameter", i );
        p1.addParameter( "BoolParameter", b );
        p1.addParameter( "UIntParameter", ui );
        p1.addParameter( "ScalarParameter", s );
        p1.addParameter( "IntsParameter", is );
        p1.addParameter( "UIntsParameter", uis );
        p1.addParameter( "ScalarsParameter", ss );
        p1.addParameter( "Vec2Parameter", vec2 );
        p1.addParameter( "Vec3Parameter", vec3 );
        p1.addParameter( "Vec4Parameter", vec4 );
        p1.addParameter( "ColorParameter", color );
        p1.addParameter( "Mat2Parameter", mat2 );
        p1.addParameter( "Mat3Parameter", mat3 );
        p1.addParameter( "Mat4Parameter", mat4 );
        p1.addParameter( "TextureParameter", &tex1, 1 );

        REQUIRE( p1.getParameterSet<RP::IntParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::BoolParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::UIntParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::ScalarParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::IntsParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::UIntsParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::ScalarsParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Vec2Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Vec3Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Vec4Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::ColorParameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Mat2Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Mat3Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::Mat4Parameter>().size() == 1 );
        REQUIRE( p1.getParameterSet<RP::TextureParameter>().size() == 1 );

        REQUIRE( p1.getParameterSet<RP::IntParameter>().at( "IntParameter" ).m_value == i );
        REQUIRE( p1.getParameterSet<RP::BoolParameter>().at( "BoolParameter" ).m_value == b );
        REQUIRE( p1.getParameterSet<RP::UIntParameter>().at( "UIntParameter" ).m_value == ui );
        REQUIRE( p1.getParameterSet<RP::ScalarParameter>().at( "ScalarParameter" ).m_value == s );
        REQUIRE( p1.getParameterSet<RP::IntsParameter>().at( "IntsParameter" ).m_value == is );
        REQUIRE( p1.getParameterSet<RP::UIntsParameter>().at( "UIntsParameter" ).m_value == uis );
        REQUIRE( p1.getParameterSet<RP::ScalarsParameter>().at( "ScalarsParameter" ).m_value ==
                 ss );
        REQUIRE( p1.getParameterSet<RP::Vec2Parameter>().at( "Vec2Parameter" ).m_value == vec2 );
        REQUIRE( p1.getParameterSet<RP::Vec3Parameter>().at( "Vec3Parameter" ).m_value == vec3 );
        REQUIRE( p1.getParameterSet<RP::Vec4Parameter>().at( "Vec4Parameter" ).m_value == vec4 );
        REQUIRE( p1.getParameterSet<RP::ColorParameter>().at( "ColorParameter" ).m_value == color );
        REQUIRE( p1.getParameterSet<RP::Mat2Parameter>().at( "Mat2Parameter" ).m_value == mat2 );
        REQUIRE( p1.getParameterSet<RP::Mat3Parameter>().at( "Mat3Parameter" ).m_value == mat3 );
        REQUIRE( p1.getParameterSet<RP::Mat4Parameter>().at( "Mat4Parameter" ).m_value == mat4 );
        REQUIRE( p1.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texture ==
                 &tex1 );
        REQUIRE( p1.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texUnit ==
                 1 );
        RP p2;
        p2.addParameter( "IntParameter", i + 1 );
        p2.addParameter( "BoolParameter", !b );
        p2.addParameter( "UIntParameter", ui + 1 );
        p2.addParameter( "ScalarParameter", s + 1_ra );
        is.push_back( 0 );
        p2.addParameter( "IntsParameter", is );
        uis.push_back( 0u );
        p2.addParameter( "UIntsParameter", uis );
        ss.push_back( 0_ra );
        p2.addParameter( "ScalarsParameter", ss );
        p2.addParameter( "Vec2Parameter", Vector2 { vec2 + Vector2 { 1_ra, 1_ra } } );
        p2.addParameter( "Vec3Parameter", Vector3 { vec3 + Vector3 { 1_ra, 1_ra, 1_ra } } );
        p2.addParameter( "Vec4Parameter", Vector4 { vec4 + Vector4 { 1_ra, 1_ra, 1_ra, 1_ra } } );
        p2.addParameter( "ColorParameter", Color::Red() );
        Texture tex2 { { "texture2" } };
        p2.addParameter( "TextureParameter", &tex2, 2 );
        p2.addParameter( "Foo", 42 );

        // add a int parameter to p1
        p1.addParameter( "Bar", 43 );

        RP kept = p1;
        kept.mergeKeepParameters( p2 );

        // existings parameters are note changes (p1's values)
        REQUIRE( kept.getParameterSet<RP::IntParameter>().at( "IntParameter" ).m_value ==
                 p1.getParameterSet<RP::IntParameter>().at( "IntParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::BoolParameter>().at( "BoolParameter" ).m_value ==
                 p1.getParameterSet<RP::BoolParameter>().at( "BoolParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::UIntParameter>().at( "UIntParameter" ).m_value ==
                 p1.getParameterSet<RP::UIntParameter>().at( "UIntParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::ScalarParameter>().at( "ScalarParameter" ).m_value ==
                 p1.getParameterSet<RP::ScalarParameter>().at( "ScalarParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::IntsParameter>().at( "IntsParameter" ).m_value ==
                 p1.getParameterSet<RP::IntsParameter>().at( "IntsParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::UIntsParameter>().at( "UIntsParameter" ).m_value ==
                 p1.getParameterSet<RP::UIntsParameter>().at( "UIntsParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::ScalarsParameter>().at( "ScalarsParameter" ).m_value ==
                 p1.getParameterSet<RP::ScalarsParameter>().at( "ScalarsParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::Vec2Parameter>().at( "Vec2Parameter" ).m_value ==
                 p1.getParameterSet<RP::Vec2Parameter>().at( "Vec2Parameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::Vec3Parameter>().at( "Vec3Parameter" ).m_value ==
                 p1.getParameterSet<RP::Vec3Parameter>().at( "Vec3Parameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::Vec4Parameter>().at( "Vec4Parameter" ).m_value ==
                 p1.getParameterSet<RP::Vec4Parameter>().at( "Vec4Parameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::ColorParameter>().at( "ColorParameter" ).m_value ==
                 p1.getParameterSet<RP::ColorParameter>().at( "ColorParameter" ).m_value );
        REQUIRE( kept.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texture ==
                 p1.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texture );
        // Foo is p2's value
        REQUIRE( kept.getParameterSet<RP::IntParameter>().at( "Foo" ).m_value ==
                 p2.getParameterSet<RP::IntParameter>().at( "Foo" ).m_value );

        // Bar is on p1 side only, still here
        REQUIRE( kept.getParameterSet<RP::IntParameter>().at( "Bar" ).m_value ==
                 p1.getParameterSet<RP::IntParameter>().at( "Bar" ).m_value );

        RP replaced = p1;
        replaced.mergeReplaceParameters( p2 );
        // Existings in p1 and p2, as well as new parameters are set to p2's values
        REQUIRE( replaced.getParameterSet<RP::IntParameter>().at( "IntParameter" ).m_value ==
                 p2.getParameterSet<RP::IntParameter>().at( "IntParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::BoolParameter>().at( "BoolParameter" ).m_value ==
                 p2.getParameterSet<RP::BoolParameter>().at( "BoolParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::UIntParameter>().at( "UIntParameter" ).m_value ==
                 p2.getParameterSet<RP::UIntParameter>().at( "UIntParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::ScalarParameter>().at( "ScalarParameter" ).m_value ==
                 p2.getParameterSet<RP::ScalarParameter>().at( "ScalarParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::IntsParameter>().at( "IntsParameter" ).m_value ==
                 p2.getParameterSet<RP::IntsParameter>().at( "IntsParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::UIntsParameter>().at( "UIntsParameter" ).m_value ==
                 p2.getParameterSet<RP::UIntsParameter>().at( "UIntsParameter" ).m_value );
        REQUIRE(
            replaced.getParameterSet<RP::ScalarsParameter>().at( "ScalarsParameter" ).m_value ==
            p2.getParameterSet<RP::ScalarsParameter>().at( "ScalarsParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::Vec2Parameter>().at( "Vec2Parameter" ).m_value ==
                 p2.getParameterSet<RP::Vec2Parameter>().at( "Vec2Parameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::Vec3Parameter>().at( "Vec3Parameter" ).m_value ==
                 p2.getParameterSet<RP::Vec3Parameter>().at( "Vec3Parameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::Vec4Parameter>().at( "Vec4Parameter" ).m_value ==
                 p2.getParameterSet<RP::Vec4Parameter>().at( "Vec4Parameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::ColorParameter>().at( "ColorParameter" ).m_value ==
                 p2.getParameterSet<RP::ColorParameter>().at( "ColorParameter" ).m_value );
        REQUIRE( replaced.getParameterSet<RP::IntParameter>().at( "Foo" ).m_value ==
                 p2.getParameterSet<RP::IntParameter>().at( "Foo" ).m_value );
        REQUIRE(
            replaced.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texture ==
            p2.getParameterSet<RP::TextureParameter>().at( "TextureParameter" ).m_texture );
        // Bar is on p1 side only and not changed
        REQUIRE( replaced.getParameterSet<RP::IntParameter>().at( "Bar" ).m_value ==
                 p1.getParameterSet<RP::IntParameter>().at( "Bar" ).m_value );
    }

    SECTION( "Enum parameter" ) {
        RP params;

        enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };
        using ValuesEnumType      = typename std::underlying_type_t<Values>;
        using ValuesParameterType = typename RP::TParameter<ValuesEnumType>;

        enum Unregistered : int { LOW = -1, MIDDDLE = 0, HIGH = 1 };
        using UnregisteredEnumType      = typename std::underlying_type_t<Unregistered>;
        using UnregisteredParameterType = typename RP::TParameter<UnregisteredEnumType>;

        auto vnc                 = new RP::EnumConverter<Values>( { { Values::VALUE_0, "VALUE_0" },
                                                                    { Values::VALUE_1, "VALUE_1" },
                                                                    { Values::VALUE_2, "VALUE_2" } } );
        auto valuesEnumConverter = std::shared_ptr<RP::EnumConverter<Values>>( vnc );

        REQUIRE( valuesEnumConverter->getEnumerator( Values::VALUE_2 ) == "VALUE_2" );
        REQUIRE( valuesEnumConverter->getEnumerator( "VALUE_1" ) == Values::VALUE_1 );
        REQUIRE( valuesEnumConverter->getEnumerators().size() == 3 );

        REQUIRE( !params.getEnumConverter( "enum.semantic" ) );
        params.addEnumConverter( "enum.semantic", valuesEnumConverter );
        REQUIRE( params.getEnumConverter( "enum.semantic" ) );
        REQUIRE( !params.getEnumConverter( "enum.unknown" ) );

        REQUIRE( params.getEnumString( "enum.semantic", Values::VALUE_0 ) == "VALUE_0" );
        REQUIRE( params.getEnumString( "enum.unknown", Unregistered::LOW ) == "" );

        params.addParameter( "enum.semantic", Values::VALUE_0 );
        auto v = params.getParameter<ValuesParameterType>( "enum.semantic" );
        REQUIRE( v.m_value == Values::VALUE_0 );
        REQUIRE( params.getEnumString( "enum.semantic", v.m_value ) == "VALUE_0" );

        valuesEnumConverter->setEnumValue( params, "enum.semantic", "VALUE_1" );
        v = params.getParameter<ValuesParameterType>( "enum.semantic" );
        REQUIRE( v.m_value == Values::VALUE_1 );
        REQUIRE( params.getEnumString( "enum.semantic", v.m_value ) == "VALUE_1" );

        params.addParameter( "enum.semantic", "VALUE_2" );
        v = params.getParameter<ValuesParameterType>( "enum.semantic" );
        REQUIRE( v.m_value == Values::VALUE_2 );
        REQUIRE( params.getEnumString( "enum.semantic", v.m_value ) == "VALUE_2" );

        params.addParameter( "enum.unknown", Unregistered::LOW );
        auto u = params.getParameter<UnregisteredParameterType>( "enum.unknown" );
        REQUIRE( u.m_value == Unregistered::LOW );
        REQUIRE( params.getEnumString( "enum.unknown", v.m_value ) == "" );

        params.addParameter( "enum.unknown", "Unregistered::HIGH" );
        u = params.getParameter<UnregisteredParameterType>( "enum.unknown" );
        REQUIRE( u.m_value == Unregistered::LOW );
    }
}
