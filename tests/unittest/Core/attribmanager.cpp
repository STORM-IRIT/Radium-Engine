#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/ContainerIntrospectionInterface.hpp>
#include <Core/Utils/Index.hpp>
#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace Ra::Core;
using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/Attibs", "[unittests][Core][Utils][Attribs]" ) {

    enum Enum1 { A, B, C };
    enum class Enum2 : char { A = 'a', B, C };

    Attrib<Scalar> attr1 { "attr float" };
    Attrib<Vector3> attr2 { "attr vec3" };
    Attrib<std::string> attr3 { "attr string" };
    Attrib<Enum1> attr4 { "attr enum1" };
    Attrib<Enum2> attr5 { "attr enum3" };
    Attrib<VectorNd> attr6 { "attr vecN" };

    SECTION( "type check" ) {
        REQUIRE( attr1.isFloat() );
        REQUIRE( !attr1.isType<int>() );
        REQUIRE( attr2.isVector3() );
        REQUIRE( attr3.isType<std::string>() );
        REQUIRE( attr4.isType<Enum1>() );
        REQUIRE( attr5.isType<Enum2>() );
        REQUIRE( !attr5.isType<char>() );
        REQUIRE( attr6.isType<VectorNd>() );

        REQUIRE( attr1.getNumberOfComponents() == 1 );
        REQUIRE( attr2.getNumberOfComponents() == 3 );
        REQUIRE( attr3.getNumberOfComponents() == 0 );
        REQUIRE( attr4.getNumberOfComponents() == 1 );
        REQUIRE( attr5.getNumberOfComponents() == 1 );
        REQUIRE( attr6.getNumberOfComponents() == 0 );

        REQUIRE( attr1.getStride() == sizeof( Scalar ) );
        REQUIRE( attr2.getStride() == sizeof( Vector3 ) );
        REQUIRE( attr3.getStride() == sizeof( std::string ) );
        REQUIRE( attr4.getStride() == sizeof( Enum1 ) );
        REQUIRE( attr5.getStride() == sizeof( Enum2 ) );
        REQUIRE( attr6.getStride() == sizeof( VectorNd ) );
    }

    SECTION( "const ref setData" ) {
        Attrib<Scalar>::Container attr1content  = { 1_ra, 2_ra, 3_ra };
        Attrib<Vector3>::Container attr2content = {
            { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra }, { 3_ra, 3_ra, 3_ra } };
        Attrib<std::string>::Container attr3content = { "one", "two", "three" };
        Attrib<Enum1>::Container attr4content       = { Enum1::A, Enum1::B, Enum1::C };
        Attrib<Enum2>::Container attr5content       = { Enum2::A, Enum2::B, Enum2::C };

        attr1.setData( attr1content );
        attr2.setData( attr2content );
        attr3.setData( attr3content );
        attr4.setData( attr4content );
        attr5.setData( attr5content );

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

        REQUIRE( attr1.getBufferSize() == 3 * sizeof( Scalar ) );
        REQUIRE( attr2.getBufferSize() == 3 * sizeof( Vector3 ) );
        REQUIRE( attr3.getBufferSize() == 3 * sizeof( std::string ) );
        REQUIRE( attr4.getBufferSize() == 3 * sizeof( Enum1 ) );
        REQUIRE( attr5.getBufferSize() == 3 * sizeof( Enum2 ) );

        VectorNd x { 2 };
        x << 1_ra, 2_ra;
        attr6.setData( { x } );
        REQUIRE( attr6.getBufferSize() == 1 * sizeof( VectorNd ) );
    }
    SECTION( "move setData" ) {
        Attrib<Scalar>::Container attr1content  = { 1_ra, 2_ra, 3_ra };
        Attrib<Vector3>::Container attr2content = {
            { 1_ra, 1_ra, 1_ra }, { 2_ra, 2_ra, 2_ra }, { 3_ra, 3_ra, 3_ra } };
        Attrib<std::string>::Container attr3content = { "one", "two", "three" };
        Attrib<Enum1>::Container attr4content       = { Enum1::A, Enum1::B, Enum1::C };
        Attrib<Enum2>::Container attr5content       = { Enum2::A, Enum2::B, Enum2::C };

        attr1.setData( std::move( attr1content ) );
        attr2.setData( std::move( attr2content ) );
        attr3.setData( std::move( attr3content ) );
        attr4.setData( std::move( attr4content ) );
        attr5.setData( std::move( attr5content ) );

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
    }
}

TEST_CASE( "Core/Utils/AttibManager", "[unittests][Core][Utils][Attribs][AttribManager]" ) {
    SECTION( "init and clear" ) {
        AttribManager m1;
        auto m1attr1 = m1.addAttrib<float>( "float" );
        auto m1attr2 = m1.addAttrib<double>( "double" );
        REQUIRE( m1.contains( "float" ) );
        REQUIRE( m1.contains( "double" ) );
        REQUIRE( m1.isValid( m1attr1 ) );
        REQUIRE( m1.isValid( m1attr2 ) );
        REQUIRE( !( m1attr1 == m1attr2 ) );
        REQUIRE( m1.getNumAttribs() == 2 );
        auto m1attr1bis = m1.findAttrib<float>( "float" );
        auto m1attr2bis = m1.findAttrib<double>( "double" );
        REQUIRE( m1.isValid( m1attr1bis ) );
        REQUIRE( m1.isValid( m1attr2bis ) );
        REQUIRE( m1attr1 == m1attr1bis );
        REQUIRE( m1attr1.idx() == m1attr1bis.idx() );
        REQUIRE( m1attr2.idx() == m1attr2bis.idx() );
        m1.clear();
        REQUIRE( !m1.contains( "float" ) );
        REQUIRE( !m1.contains( "double" ) );
        REQUIRE( !m1.isValid( m1attr1 ) );
        REQUIRE( !m1.isValid( m1attr2 ) );
        REQUIRE( m1.getNumAttribs() == 0 );
        m1attr1bis = m1.findAttrib<float>( "float" );
        m1attr2bis = m1.findAttrib<double>( "double" );
        REQUIRE( !m1.isValid( m1attr1bis ) );
        REQUIRE( !m1.isValid( m1attr2bis ) );
    }
    SECTION( "init and remove" ) {
        AttribManager m1;
        auto m1attr1 = m1.addAttrib<float>( "float" );
        auto m1attr2 = m1.addAttrib<double>( "double" );
        REQUIRE( m1.contains( "float" ) );
        REQUIRE( m1.contains( "double" ) );
        REQUIRE( m1.isValid( m1attr1 ) );
        REQUIRE( m1.isValid( m1attr2 ) );
        m1.removeAttrib( m1attr1 );
        REQUIRE( !m1.contains( "float" ) );
        REQUIRE( m1.contains( "double" ) );
        REQUIRE( !m1.isValid( m1attr1 ) );
        REQUIRE( m1.isValid( m1attr2 ) );
        REQUIRE( m1.getNumAttribs() == 1 );
        auto m1attr1bis = m1.findAttrib<float>( "float" );
        auto m1attr2bis = m1.findAttrib<double>( "double" );
        REQUIRE( !m1.isValid( m1attr1bis ) );
        REQUIRE( m1.isValid( m1attr2bis ) );
        REQUIRE( m1attr2bis.idx() == m1attr2.idx() );
    }
    SECTION( "get attrib and data" ) {
        AttribManager m1;
        const AttribManager& m1Const = m1;

        auto m1attr1  = m1.addAttrib<Vector3>( "vec3" );
        auto attr1ptr = m1.getAttribPtr( m1attr1 );

        auto basePtrFromName               = m1.getAttribBase( "vec3" );
        auto basePtrFromIdx                = m1.getAttribBase( m1attr1.idx() );
        auto invalidPtrFromName            = m1.getAttribBase( "invalid" );
        auto invalidPtrFromIdx             = m1.getAttribBase( Index::Invalid() );
        const auto baseConstPtrFromName    = m1Const.getAttribBase( "vec3" );
        const auto baseConstPtrFromIdx     = m1Const.getAttribBase( m1attr1.idx() );
        const auto invalidConstPtrFromName = m1Const.getAttribBase( "invalid" );
        const auto invalidConstPtrFromIdx  = m1Const.getAttribBase( Index::Invalid() );

        REQUIRE( basePtrFromName == attr1ptr );
        REQUIRE( baseConstPtrFromName == attr1ptr );
        REQUIRE( basePtrFromIdx == attr1ptr );
        REQUIRE( baseConstPtrFromIdx == attr1ptr );
        REQUIRE( invalidPtrFromName == nullptr );
        REQUIRE( invalidConstPtrFromName == nullptr );
        REQUIRE( invalidPtrFromIdx == nullptr );
        REQUIRE( invalidConstPtrFromIdx == nullptr );
    }
}
