#include <catch2/catch.hpp>

#include <Engine/Data/EnvironmentTexture.hpp>
#include <Engine/RadiumEngine.hpp>

using namespace Ra::Engine::Data;

TEST_CASE( "Engine/Data/EnvironmentTexture/Equirectangular",
           "[Engine][Engine/Data][EnvironmentTexture][equirectangular]" ) {
    SECTION( "Create environment texture" ) {
        EnvironmentTexture tex { "data/studio_garden_2k.exr", true };

        REQUIRE( tex.getImageType() == EnvironmentTexture::EnvMapType::ENVMAP_LATLON );
        REQUIRE( tex.isSkybox() );

        auto redShCoefs            = tex.getShMatrix( 0 );
        Ra::Core::Matrix4 refCoefs = ( Ra::Core::Matrix4() << -0.514535,
                                       1.08883,
                                       -0.923322,
                                       0.731273,
                                       1.08883,
                                       0.514535,
                                       -1.52304,
                                       1.33027,
                                       -0.923322,
                                       -1.52304,
                                       0.107396,
                                       -1.16553,
                                       0.731273,
                                       1.33027,
                                       -1.16553,
                                       2.17916 )
                                         .finished();
        auto diff = redShCoefs - refCoefs;
        REQUIRE( diff.norm() <= 1e-3 );
    }
}

TEST_CASE( "Engine/Data/EnvironmentTexture/PFM",
           "[Engine][Engine/Data][EnvironmentTexture][PFM]" ) {
    SECTION( "Create environment texture" ) {
        EnvironmentTexture tex { "data/uffizi_cross.pfm" };

        REQUIRE( tex.getImageType() == EnvironmentTexture::EnvMapType::ENVMAP_PFM );
        REQUIRE( !tex.isSkybox() );

        auto greenShCoefs          = tex.getShMatrix( 1 );
        Ra::Core::Matrix4 refCoefs = ( Ra::Core::Matrix4() << -1.53315,
                                       0.036986,
                                       0.0020611,
                                       0.0118119,
                                       0.036986,
                                       1.53315,
                                       0.0824425,
                                       1.87627,
                                       0.0020611,
                                       0.0824425,
                                       -0.524398,
                                       0.0441319,
                                       0.0118119,
                                       1.87627,
                                       0.0441319,
                                       2.90152 )
                                         .finished();
        auto diff = greenShCoefs - refCoefs;
        REQUIRE( diff.norm() <= 1e-3 );
    }
}
TEST_CASE( "Engine/Data/EnvironmentTexture/CubeMap",
           "[Engine][Engine/Data][EnvironmentTexture][CubeMap]" ) {
    SECTION( "Create environment texture" ) {
        EnvironmentTexture tex {
            "data/smallpark/negx.jpg;data/smallpark/negy.jpg;data/smallpark/negz.jpg;"
            "data/smallpark/posx.jpg;data/smallpark/posy.jpg;data/smallpark/posz.jpg" };

        REQUIRE( tex.getImageType() == EnvironmentTexture::EnvMapType::ENVMAP_CUBE );
        REQUIRE( !tex.isSkybox() );

        auto blueShCoefs           = tex.getShMatrix( 2 );
        Ra::Core::Matrix4 refCoefs = ( Ra::Core::Matrix4() << -0.0150307,
                                       -0.0389637,
                                       -0.0589842,
                                       -0.0352475,
                                       -0.0389637,
                                       0.0150307,
                                       -0.014953,
                                       0.30222,
                                       -0.0589842,
                                       -0.014953,
                                       -0.0621052,
                                       -0.0119704,
                                       -0.0352475,
                                       0.30222,
                                       -0.0119704,
                                       0.585438 )
                                         .finished();
        auto diff = blueShCoefs - refCoefs;
        REQUIRE( diff.norm() <= 1e-3 );
    }
}
