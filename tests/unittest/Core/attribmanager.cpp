#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>

#include <catch2/catch.hpp>

using namespace Ra::Core;
using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/Attibs", "[Core][Utils][Attribs]" ) {

    enum Enum1 { A, B, C };
    enum class Enum2 : char { A = 'a', B, C };

    Attrib<Scalar> attr1 { "attr float" };
    Attrib<Vector3> attr2 { "attr vec3" };
    Attrib<std::string> attr3 { "attr string" };
    Attrib<Enum1> attr4 { "attr enum1" };
    Attrib<Enum2> attr5 { "attr enum3" };

    attr1.setData( { 1_ra, 2_ra, 3_ra } );
    attr2.setData( { { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra }, { 3_ra, 3_ra, 3_ra } } );
    attr3.setData( { "one", "two", "three" } );
    attr4.setData( { Enum1::A, Enum1::B, Enum1::C } );
    attr5.setData( { Enum2::A, Enum2::B, Enum2::C } );

    REQUIRE( attr1.getSize() == 3 );
    REQUIRE( attr2.getSize() == 3 );
    REQUIRE( attr3.getSize() == 3 );
    REQUIRE( attr4.getSize() == 3 );
    REQUIRE( attr5.getSize() == 3 );

    auto& cont1 = attr1.data();
    auto& cont2 = attr2.data();
    auto& cont3 = attr3.data();
    auto& cont4 = attr4.data();
    auto& cont5 = attr5.data();
    REQUIRE( cont1.data() == attr1.dataPtr() );
    REQUIRE( cont2.data() == attr2.dataPtr() );
    REQUIRE( cont3.data() == attr3.dataPtr() );
    REQUIRE( cont4.data() == attr4.dataPtr() );
    REQUIRE( cont5.data() == attr5.dataPtr() );

    REQUIRE( attr1.isFloat() );
    REQUIRE( !attr1.isType<int>() );
    REQUIRE( attr2.isVector3() );
    REQUIRE( attr3.isType<std::string>() );
    REQUIRE( attr4.isType<Enum1>() );
    REQUIRE( attr5.isType<Enum2>() );
    REQUIRE( !attr5.isType<char>() );
}
