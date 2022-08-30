#include <catch2/catch.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/LambertianMaterial.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/Data/VolumetricMaterial.hpp>
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

        /* Testing default values */
        REQUIRE( bp.m_alpha == 1.0 );
        REQUIRE( !bp.m_renderAsSplat );

        /* Setting GL Parameters */
        bp.updateGL();
        auto& bpParameters = bp.getParameters();

        REQUIRE( bpParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.renderAsSplat" ) );
        REQUIRE(
            bpParameters.containsParameter<RenderParameters::ScalarParameter>( "material.alpha" ) );
        auto& ras =
            bpParameters.getParameter<RenderParameters::BoolParameter>( "material.renderAsSplat" );
        REQUIRE( ras.m_value == bp.m_renderAsSplat );

        auto& alp =
            bpParameters.getParameter<RenderParameters::ScalarParameter>( "material.alpha" );
        REQUIRE( alp.m_value == bp.m_alpha );

        /* changing parameter values */
        bpParameters.addParameter( "material.renderAsSplat", !ras.m_value );
        bpParameters.addParameter( "material.alpha", 0.5_ra );
        REQUIRE( ras.m_value != bp.m_renderAsSplat );
        REQUIRE( alp.m_value != bp.m_alpha );

        /* Updating material parameters from GL parameters */
        bp.updateFromParameters();
        REQUIRE( ras.m_value == bp.m_renderAsSplat );
        REQUIRE( alp.m_value == bp.m_alpha );
    }

    SECTION( "Lambertian material" ) {
        REQUIRE( code == 0 );
        LambertianMaterial mat( "test LambertianMaterial" );

        REQUIRE( !mat.m_perVertexColor );

        mat.updateGL();
        auto& matParameters = mat.getParameters();

        REQUIRE( matParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" ) );
        auto& pvc = matParameters.getParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" );
        REQUIRE( pvc.m_value == mat.m_perVertexColor );

        /* changing parameter values */
        matParameters.addParameter( "material.perVertexColor", !pvc.m_value );
        REQUIRE( pvc.m_value != mat.m_perVertexColor );

        /* Updating material parameters from GL parameters */
        mat.updateFromParameters();
        REQUIRE( pvc.m_value == mat.m_perVertexColor );
    }

    SECTION( "Plain material" ) {
        REQUIRE( code == 0 );
        PlainMaterial mat( "test PlainMaterial" );

        REQUIRE( !mat.m_perVertexColor );

        mat.updateGL();
        auto& matParameters = mat.getParameters();
        REQUIRE( matParameters.containsParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" ) );

        auto& pvc = matParameters.getParameter<RenderParameters::BoolParameter>(
            "material.perVertexColor" );
        REQUIRE( pvc.m_value == mat.m_perVertexColor );

        /* changing parameter values */
        matParameters.addParameter( "material.perVertexColor", !pvc.m_value );
        REQUIRE( pvc.m_value != mat.m_perVertexColor );

        /* Updating material parameters from GL parameters */
        mat.updateFromParameters();
        REQUIRE( pvc.m_value == mat.m_perVertexColor );
    }

    SECTION( "Volumetric material" ) {
        REQUIRE( code == 0 );
        VolumetricMaterial mat( "test VolumetricMaterial" );
        float d = 1.f;
        Texture density { { "simpleDensity",
                            gl::GL_TEXTURE_3D,
                            1,
                            1,
                            1,
                            gl::GL_RED,
                            gl::GL_RED,
                            gl::GL_FLOAT,
                            gl::GL_CLAMP_TO_EDGE,
                            gl::GL_CLAMP_TO_EDGE,
                            gl::GL_CLAMP_TO_EDGE,
                            gl::GL_NEAREST,
                            gl::GL_NEAREST,
                            &d } };
        mat.setTexture( &density );

        REQUIRE( mat.m_g == 0_ra );

        mat.updateGL();
        auto& matParameters = mat.getParameters();
        REQUIRE(
            matParameters.containsParameter<RenderParameters::ScalarParameter>( "material.g" ) );

        auto& g = matParameters.getParameter<RenderParameters::ScalarParameter>( "material.g" );
        REQUIRE( g.m_value == 0_ra );

        /* changing parameter values */
        matParameters.addParameter( "material.g", 0.5_ra );
        REQUIRE( g.m_value != mat.m_g );

        /* Updating material parameters from GL parameters */
        mat.updateFromParameters();
        REQUIRE( g.m_value == mat.m_g );
    }

    SECTION( "Metadata verification" ) {
        BlinnPhongMaterial bp( "testBlinnPhong" );
        auto bpMetadata = bp.getParametersMetadata();
        REQUIRE( bpMetadata.contains( "material.tex.ns" ) );
        REQUIRE( bpMetadata["material.tex.ns"].contains( "type" ) );
        REQUIRE( bpMetadata["material.tex.ns"]["type"] == "texture" );

        LambertianMaterial lm( "test LambertianMaterial" );
        auto lmMetadata = lm.getParametersMetadata();
        REQUIRE( lmMetadata.contains( "material.tex.color" ) );
        REQUIRE( lmMetadata["material.tex.color"].contains( "type" ) );
        REQUIRE( lmMetadata["material.tex.color"]["type"] == "texture" );

        PlainMaterial pm( "test PlainMaterial" );
        auto pmMetadata = pm.getParametersMetadata();
        REQUIRE( pmMetadata.contains( "material.tex.color" ) );
        REQUIRE( pmMetadata["material.tex.color"].contains( "type" ) );
        REQUIRE( pmMetadata["material.tex.color"]["type"] == "texture" );

        VolumetricMaterial vm( "test VolumetricMaterial" );
        auto vmMetadata = vm.getParametersMetadata();
        REQUIRE( vmMetadata.contains( "material.sigma_a" ) );
        REQUIRE( vmMetadata["material.sigma_a"].contains( "type" ) );
        REQUIRE( vmMetadata["material.sigma_a"]["type"] == "array" );
    }
}
