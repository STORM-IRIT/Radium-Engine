#include <catch2/catch.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/LambertianMaterial.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/RadiumEngine.hpp>

#include <Headless/CLIViewer.hpp>

using namespace Ra::Headless;
using namespace Ra::Engine::Data;

TEST_CASE( "Engine/Data/Materials", "[Engine][Engine/Data][Materials]" ) {

    // Get the Engine and materials initialized
    glbinding::Version glVersion { 4, 4 };
    CLIViewer viewer { glVersion };
    const char* testName = "Materials testing";
    auto code            = viewer.init( 1, &testName );

    SECTION( "Blinn-Phong material" ) {
        REQUIRE( code == 0 );
        BlinnPhongMaterial bp( "testBlinnPhong" );

        REQUIRE( bp.m_alpha == 1.0 );
        REQUIRE( !bp.m_renderAsSplat );

        bp.updateGL();
        auto bpParameters = bp.getParameters();
        /* The method containsParameter and getParameter will be added in PR #950 */
        /*
        REQUIRE( bpParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.renderAsSplat" ) );
        REQUIRE(
            bpParameters.containsParameter<RenderParameters::ScalarParameter>( "material.alpha" ) );

        auto ras =
            bpParameters.getParameter<RenderParameters::BoolParameter>( "material.renderAsSplat" );
        REQUIRE( ras.m_value == bp.m_renderAsSplat );

        auto alp = bpParameters.getParameter<RenderParameters::ScalarParameter>( "material.alpha" );
        REQUIRE( alp.m_value == bp.m_alpha );
        */
    }

    SECTION( "Lambertian material" ) {
        REQUIRE( code == 0 );
        LambertianMaterial mat( "test LambertianMaterial" );

        REQUIRE( !mat.m_perVertexColor );

        mat.updateGL();
        auto matParameters = mat.getParameters();
        /* The method containsParameter and getParameter will be added in PR #950 */
        /*
        REQUIRE( matParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" ) );

        auto pvc = matParameters.getParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" );
        REQUIRE( pvc.m_value == mat.m_perVertexColor );
        */
    }

    SECTION( "Plain material" ) {
        REQUIRE( code == 0 );
        PlainMaterial mat( "test PlainMaterial" );

        REQUIRE( !mat.m_perVertexColor );

        mat.updateGL();
        auto matParameters = mat.getParameters();
        /* The method containsParameter and getParameter will be added in PR #950 */
        /*
        REQUIRE( matParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" ) );

        auto pvc = matParameters.getParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" );
        REQUIRE( pvc.m_value == mat.m_perVertexColor );
        */
    }
}
