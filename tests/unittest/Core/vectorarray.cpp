#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <type_traits>

using namespace Ra::Core;

TEST_CASE( "Core/Container/VectorArray", "[unittests][Core][Container][VectorArray]" ) {
    enum class MyEnum : int { A, B, C };
    // type check
    REQUIRE( VectorArrayTypeHelper<float>::NumberOfComponents == 1 );
    REQUIRE( VectorArrayTypeHelper<Vector3>::NumberOfComponents == 3 );
    REQUIRE( VectorArrayTypeHelper<MyEnum>::NumberOfComponents == 1 );
    REQUIRE( VectorArrayTypeHelper<std::string>::NumberOfComponents == 0 );
    REQUIRE( VectorArrayTypeHelper<VectorN>::NumberOfComponents == -1 );
    REQUIRE( VectorArrayTypeHelper<int*>::NumberOfComponents == 0 );
    REQUIRE( VectorArrayTypeHelper<std::function<int( float, float )>>::NumberOfComponents == 0 );

    REQUIRE( std::is_same<VectorArray<float>::component_type, float>::value );
    REQUIRE( std::is_same<VectorArray<Scalar>::component_type, Scalar>::value );
    REQUIRE( std::is_same<VectorArray<Vector3>::component_type, Scalar>::value );
    REQUIRE( std::is_same<VectorArray<MyEnum>::component_type, MyEnum>::value );
    REQUIRE( std::is_same<VectorArray<std::string>::component_type, std::string>::value );
    REQUIRE( std::is_same<VectorArray<VectorN>::component_type, Scalar>::value );
    REQUIRE( std::is_same<VectorArray<int*>::component_type, int*>::value );
    REQUIRE( std::is_same<VectorArray<std::function<int( float, float )>>::component_type,
                          std::function<int( float, float )>>::value );

    VectorArray<float> floatArray;
    VectorArray<Scalar> scalarArray;
    VectorArray<Vector3> vec3Array;
    VectorArray<MyEnum> enumArray;
    VectorArray<std::string> stringArray;
    VectorArray<VectorN> vecDynArray;
    VectorArray<int*> intPtrArray;
    VectorArray<std::function<int( float, float )>> funArray;

    REQUIRE( floatArray.getSize() == 0 );
    REQUIRE( scalarArray.getSize() == 0 );
    REQUIRE( vec3Array.getSize() == 0 );
    REQUIRE( enumArray.getSize() == 0 );
    REQUIRE( stringArray.getSize() == 0 );
    REQUIRE( vecDynArray.getSize() == 0 );
    REQUIRE( intPtrArray.getSize() == 0 );
    REQUIRE( funArray.getSize() == 0 );

    REQUIRE( floatArray.getBufferSize() == 0 );
    REQUIRE( scalarArray.getBufferSize() == 0 );
    REQUIRE( vec3Array.getBufferSize() == 0 );
    REQUIRE( enumArray.getBufferSize() == 0 );
    REQUIRE( stringArray.getBufferSize() == 0 );
    REQUIRE( vecDynArray.getBufferSize() == 0 );
    REQUIRE( intPtrArray.getBufferSize() == 0 );
    REQUIRE( funArray.getBufferSize() == 0 );

    floatArray.push_back( 1.f );
    scalarArray.push_back( 1_ra );
    vec3Array.emplace_back( 1_ra, 2_ra, 3_ra );
    enumArray.push_back( MyEnum::B );
    stringArray.push_back( "foo" );
    vecDynArray.emplace_back( VectorN {} );
    intPtrArray.push_back( nullptr );
    funArray.push_back( []( float a, float b ) -> int { return a + b; } );

    REQUIRE( floatArray.getSize() == 1 );
    REQUIRE( scalarArray.getSize() == 1 );
    REQUIRE( vec3Array.getSize() == 1 );
    REQUIRE( enumArray.getSize() == 1 );
    REQUIRE( stringArray.getSize() == 1 );
    REQUIRE( vecDynArray.getSize() == 1 );
    REQUIRE( intPtrArray.getSize() == 1 );
    REQUIRE( funArray.getSize() == 1 );

    auto floatMap  = floatArray.getMap();
    auto scalarMap = scalarArray.getMap();
    auto vec3Map   = vec3Array.getMap();
    auto enumMap   = enumArray.getMap();
    REQUIRE( floatMap( 0 ) == 1.f );
    REQUIRE( scalarMap( 0 ) == 1_ra );
    REQUIRE( vec3Map.col( 0 ) == Vector3 { 1_ra, 2_ra, 3_ra } );
    REQUIRE( enumMap( 0 ) == MyEnum::B );

    const auto& floatConstArray  = floatArray;
    const auto& scalarConstArray = scalarArray;
    const auto& vec3ConstArray   = vec3Array;
    const auto& enumConstArray   = enumArray;

    const auto floatConstMap  = floatConstArray.getMap();
    const auto scalarConstMap = scalarConstArray.getMap();
    const auto vec3ConstMap   = vec3ConstArray.getMap();
    const auto enumConstMap   = enumConstArray.getMap();
    REQUIRE( floatConstMap( 0 ) == 1.f );
    REQUIRE( scalarConstMap( 0 ) == 1_ra );
    REQUIRE( vec3ConstMap.col( 0 ) == Vector3 { 1_ra, 2_ra, 3_ra } );
    REQUIRE( enumConstMap( 0 ) == MyEnum::B );

    // the following lines are not allowed (std::enable_if fails at compile time)
    // auto vecDynMap = vecDynArray.getMap();
    // auto intPtrMap = intPtrArray.getMap();
    // auto funMap    = funArray.getMap();
    // vecDynMap( 0 );
    // intPtrMap( 0 );
    // funMap( 0 );
}
