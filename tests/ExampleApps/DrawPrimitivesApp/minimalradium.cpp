
#include <minimalradium.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/LambertianMaterial.hpp>
#include <Engine/Renderer/Material/PlainMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <random>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.

MinimalComponent::MinimalComponent( Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( "Minimal Component", entity ) {}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {

    using namespace Ra::Core;
    using namespace Ra::Engine;

    ///
    // basic render technique associated with all object here, they use per vertex kd.
    RenderTechnique shadedRt;
    {
        auto mat              = make_shared<BlinnPhongMaterial>( "Shaded Material" );
        mat->m_perVertexColor = true;
        mat->m_ks             = Utils::Color::White();
        mat->m_ns             = 100_ra;

        auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( shadedRt, false );
        shadedRt.setParametersProvider( mat );
    }
    RenderTechnique plainRt;
    {
        auto mat              = make_shared<PlainMaterial>( "Plain Material" );
        mat->m_perVertexColor = true;

        auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "Plain" );
        builder.second( plainRt, false );
        plainRt.setParametersProvider( mat );
    }
    RenderTechnique lambertianRt;
    {
        auto mat              = make_shared<LambertianMaterial>( "Lambertian Material" );
        mat->m_perVertexColor = true;

        auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "Lambertian" );
        builder.second( lambertianRt, false );
        lambertianRt.setParametersProvider( mat );
    }

    //// setup ////
    Scalar colorBoost = 1_ra; /// since simple primitive are ambient only, boost their color
    Scalar cellSize   = 0.25_ra;
    Vector3 cellCorner {-1_ra, 0_ra, 0.25_ra};
    Scalar offset {0.05_ra};
    Vector3 offsetVec {offset, offset, offset};
    std::random_device rd;    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen( rd() ); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<Scalar> dis015( 0_ra, cellSize - 2_ra * offset );
    std::uniform_real_distribution<Scalar> dis01( 0_ra, 1_ra );
    std::uniform_real_distribution<Scalar> dis11( -1_ra, 1_ra );
    std::uniform_int_distribution<uint> disInt( 0, 128 );

    //// GRID ////
    {
        RenderTechnique gridRt;
        // Plain shader
        auto builder = EngineRenderTechniques::getDefaultTechnique( "Plain" );
        builder.second( gridRt, false );
        auto mat              = Ra::Core::make_shared<PlainMaterial>( "Grid material" );
        mat->m_perVertexColor = true;
        gridRt.setParametersProvider( mat );

        auto gridPrimitive = DrawPrimitives::Grid( Vector3::Zero(),
                                                   Vector3::UnitX(),
                                                   Vector3::UnitZ(),
                                                   Utils::Color::Grey( 0.6f ),
                                                   cellSize,
                                                   8 );

        auto gridRo = RenderObject::createRenderObject(
            "test_grid", this, RenderObjectType::Geometry, gridPrimitive, gridRt );
        gridRo->setPickable( false );
        addRenderObject( gridRo );
    }

    //// CUBES ////
    std::shared_ptr<Ra::Engine::Mesh> cube1( new Ra::Engine::Mesh( "Cube" ) );
    cube1->loadGeometry( Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    cube1->getCoreGeometry().addAttrib(
        "in_color", Vector4Array {cube1->getNumVertices(), Utils::Color::Green()} );

    auto renderObject1 = RenderObject::createRenderObject(
        "CubeRO_1", this, RenderObjectType::Geometry, cube1, shadedRt );
    renderObject1->setLocalTransform(
        Transform {Translation( Vector3( 3 * cellSize, 0_ra, 0_ra ) )} );

    addRenderObject( renderObject1 );

    // another cube
    std::shared_ptr<Ra::Engine::Mesh> cube2( new Ra::Engine::Mesh( "Cube" ) );
    cube2->loadGeometry( Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    cube2->getCoreGeometry().addAttrib(
        "colour", Vector4Array {cube2->getNumVertices(), Utils::Color::Red()} );

    cube2->setAttribNameCorrespondance( "colour", "in_color" );
    auto renderObject2 = RenderObject::createRenderObject(
        "CubeRO_2", this, RenderObjectType::Geometry, cube2, lambertianRt );
    renderObject2->setLocalTransform(
        Transform {Translation( Vector3( 4 * cellSize, 0_ra, 0_ra ) )} );

    addRenderObject( renderObject2 );

    //// POINTS ////
    cellCorner = {-1_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_point",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Point( cellCorner, colorBoost * Utils::Color {0_ra, 1_ra, 0.3_ra} ),
        plainRt ) );

    for ( int i = 0; i < 10; ++i )
    {
        Vector3 randomVec {cellCorner + offsetVec +
                           Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
        addRenderObject( RenderObject::createRenderObject(
            "test_point",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Point( randomVec, colorBoost * randomCol, 0.03_ra ),
            plainRt ) );
    }

    //// LINES ////
    cellCorner = {-0.75_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_line",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Line( cellCorner,
                              cellCorner + Vector3 {0_ra, 0.4_ra, 0_ra},
                              colorBoost * Utils::Color::Red() ),
        plainRt ) );

    for ( int i = 0; i < 20; ++i )
    {
        Vector3 randomVec1 {cellCorner + offsetVec +
                            Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
        Vector3 randomVec2 {cellCorner + offsetVec +
                            Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

        addRenderObject( RenderObject::createRenderObject(
            "test_line",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Line( randomVec1, randomVec2, colorBoost * randomCol ),
            plainRt ) );
    }

    //// VECTOR ////
    cellCorner = {-0.5_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_vector",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Vector(
            cellCorner, Vector3 {0_ra, 0.5_ra, 0_ra}, colorBoost * Utils::Color::Blue() ),
        plainRt ) );

    for ( int i = 0; i < 10; ++i )
    {
        Vector3 randomVec1 {cellCorner + offsetVec +
                            Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
        Vector3 randomVec2 {cellCorner + offsetVec +
                            Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

        addRenderObject( RenderObject::createRenderObject(
            "test_vector",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Vector( randomVec1, randomVec2 - randomVec1, colorBoost * randomCol ),
            plainRt ) );
    }

    /// RAY ////
    cellCorner = {-0.25_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_ray",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Ray(
            {cellCorner, {0_ra, 1_ra, 0_ra}}, colorBoost * Utils::Color::Yellow(), cellSize ),
        plainRt ) );

    //// TRIANGLES ////
    cellCorner = {0_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_triangle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Triangle( cellCorner + Vector3 {-0.01_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3 {+0.01_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3 {+0.0_ra, 0.02_ra, 0.0_ra},
                                  colorBoost * Utils::Color::White(),
                                  true ),
        shadedRt ) );

    cellCorner = {0_ra + 0.125_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_triangle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Triangle( cellCorner + Vector3 {-0.071_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3 {+0.071_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3 {+0.0_ra, 0.2_ra, 0.0_ra},
                                  colorBoost * Utils::Color::Green(),
                                  true ),
        shadedRt ) );

    for ( int i = 0; i < 10; ++i )
    {

        addRenderObject( RenderObject::createRenderObject(
            "test_triangle_wire",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle(
                cellCorner + Vector3 {-0.071_ra, 0.0_ra, Scalar( i ) / 10_ra * cellSize},
                cellCorner + Vector3 {+0.071_ra, 0.0_ra, Scalar( i ) / 10_ra * cellSize},
                cellCorner + Vector3 {+0.0_ra, 0.2_ra, Scalar( i ) / 10_ra * cellSize},
                colorBoost * Utils::Color::White() * Scalar( i ) / 10_ra,
                false ),
            plainRt ) );
    }

    /*
    addRenderObject(
        RenderObject::createRenderObject( "test_quad_strip",
                                          this,
                                          RenderObjectType::Geometry,
                                          DrawPrimitives::QuadStrip( {0.1_ra, 0.0_ra, -1.0_ra},
                                                                     {0.3_ra, 0.0_ra, 0.1_ra},
                                                                     {-0.1_ra, 0.3_ra, 0.1_ra},
                                                                     6,
                                                                     {0.7_ra, 0.2_ra, 0.9_ra} ) ) );
    */

    //// CIRCLE ////
    cellCorner = {0.25_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_circle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Circle( cellCorner,
                                {0_ra, 0_ra, 1_ra},
                                cellSize / 8_ra,
                                64,
                                colorBoost * Utils::Color::White() ),
        plainRt ) );

    uint end = 8;
    for ( uint j = 0; j < end; ++j )
        for ( uint i = 0; i < end; ++i )
        {
            Vector3 circleCenter {cellCorner + offsetVec +
                                  Vector3 {Scalar( j ) / end * ( cellSize - 2 * offset ),
                                           offset,
                                           Scalar( i ) / end * ( cellSize - 2 * offset )}};
            Vector3 circleNormal {Vector3 {Scalar( i ), Scalar( j ), 10_ra}};
            circleNormal.normalize();
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
            Scalar circleRadius {Scalar( end / 2 + i ) / Scalar( 2 * end ) * cellSize / 8_ra};
            uint circleSubdiv {3 + j * end + i};

            addRenderObject(
                RenderObject::createRenderObject( "test_circle",
                                                  this,
                                                  RenderObjectType::Geometry,
                                                  DrawPrimitives::Circle( circleCenter,
                                                                          circleNormal,
                                                                          circleRadius,
                                                                          circleSubdiv,
                                                                          colorBoost * randomCol ),
                                                  plainRt ) );
        }

    //// CIRCLE ARC ////

    addRenderObject( RenderObject::createRenderObject(
        "test_circle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::CircleArc( cellCorner + Vector3 {0_ra, 2_ra * offset, 0_ra},
                                   {0_ra, 0_ra, 1_ra},
                                   cellSize / 8_ra,
                                   1_ra,
                                   64,
                                   colorBoost * Utils::Color::White() ),
        plainRt ) );

    for ( uint j = 0; j < end; ++j )
        for ( uint i = 0; i < end; ++i )
        {
            Vector3 circleCenter {cellCorner + offsetVec +
                                  Vector3 {Scalar( j ) / end * ( cellSize - 2 * offset ),
                                           2 * offset,
                                           Scalar( i ) / end * ( cellSize - 2 * offset )}};
            Vector3 circleNormal {0_ra, 0_ra, 1_ra};
            circleNormal.normalize();
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
            Scalar circleRadius {( cellSize - 2_ra * offset ) / 20_ra};
            Scalar circleArc {Scalar( i ) / Scalar( end ) * 2_ra};
            uint circleSubdiv {2 + j};

            addRenderObject( RenderObject::createRenderObject(
                "test_circle",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::CircleArc( circleCenter,
                                           circleNormal,
                                           circleRadius,
                                           circleArc,
                                           circleSubdiv,
                                           colorBoost * randomCol ),
                plainRt ) );
        }

    //// SPHERE /////

    cellCorner = {-1._ra, 0_ra, 0.25_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_sphere",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Sphere( cellCorner, 0.02_ra, Utils::Color::White() ),
        shadedRt ) );

    end = 32;
    for ( uint i = 0; i < end; ++i )
    {
        Scalar angle {Scalar( i ) / Scalar( end ) * 7_ra};
        Scalar ratio {Scalar( i ) / Scalar( end - 1 )};
        Vector3 center {cellCorner + Vector3 {cellSize / 2_ra, ratio * .1_ra, cellSize / 2_ra}};
        Vector3 center1 {center + Vector3 {ratio * cellSize * .4_ra * std::cos( angle ),
                                           0_ra,
                                           ratio * cellSize * .4_ra * std::sin( angle )}};
        Vector3 center2 {
            center + Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 2_ra ),
                              0_ra,
                              ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 2_ra )}};

        Vector3 center3 {
            center + Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 4_ra ),
                              0_ra,
                              ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 4_ra )}};

        Color color1 {Utils::Color::Green() * ratio};
        Color color2 {Utils::Color::Red() * ratio};
        Color color3 {Utils::Color::Blue() * ratio};

        addRenderObject( RenderObject::createRenderObject(
            "test_sphere",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Sphere( center1, 0.005_ra + ratio * 0.01_ra, color1 ),
            shadedRt ) );
        addRenderObject( RenderObject::createRenderObject(
            "test_sphere",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Sphere( center2, 0.005_ra + ratio * 0.01_ra, color2 ),
            shadedRt ) );

        addRenderObject( RenderObject::createRenderObject(
            "test_sphere",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Sphere( center3, 0.01_ra + ratio * 0.01_ra, color3 ),
            shadedRt ) );
    }

    //// CAPSULE ////
    cellCorner = {-0.75_ra, 0_ra, 0.25_ra};

    addRenderObject( RenderObject::createRenderObject(
        "test_capsule",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Capsule(
            cellCorner, cellCorner + Vector3 {0_ra, 0.1_ra, 0_ra}, 0.02_ra, Utils::Color::White() ),
        shadedRt ) );

    //// DISK ////
    cellCorner = {-0.5_ra, 0_ra, 0.25_ra};

    addRenderObject( RenderObject::createRenderObject(
        "test_disk",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Disk( cellCorner,
                              Vector3 {0_ra, 0_ra, 1_ra},
                              0.05_ra,
                              32,
                              colorBoost * Utils::Color::White() ),
        shadedRt ) );

    /// NORMAL
    cellCorner = {-0.5_ra, 0_ra, 0.25_ra};

    addRenderObject( RenderObject::createRenderObject(
        "test_normal",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Normal(
            cellCorner, Vector3 {0_ra, 0_ra, 1_ra}, colorBoost * Utils::Color::White() ),
        plainRt ) );

    /*
        addRenderObject( RenderObject::createRenderObject(
        "test_ray",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Frame( const Core::Transform& frameFromEntity,
            Scalar scale = 0.1f );

                           addRenderObject( RenderObject::createRenderObject(
                   "test_ray",
                   this,
                   RenderObjectType::Geometry,
                   DrawPrimitives::Grid( const Core::Vector3& center,
                                                   const Core::Vector3& x,
                                                   const Core::Vector3& y,
                                                   const Core::Utils::Color& color,
                                                   Scalar cellSize = 1.f,
                                                   uint res        = 10 );

                           addRenderObject( RenderObject::createRenderObject(
                   "test_ray",
                   this,
                   RenderObjectType::Geometry,
                   DrawPrimitives::AABB( const Core::Aabb& aabb, const
                       Core::Utils::Color& color );

                           addRenderObject( RenderObject::createRenderObject(
                   "test_ray",
                   this,
                   RenderObjectType::Geometry,
                   DrawPrimitives::OBB( const Core::Geometry::Obb& obb, const
                       Core::Utils::Color& color );

                           addRenderObject( RenderObject::createRenderObject(
                   "test_ray",
                   this,
                   RenderObjectType::Geometry,
                   DrawPrimitives::Spline( const Core::Geometry::Spline<3, 3>&
                       spline, uint pointCount, const Core::Utils::Color& color, Scalar scale
       = 1.0f
                   );*/
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    CORE_UNUSED( info );
    CORE_UNUSED( q );

    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    //    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );
}

void MinimalSystem::addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
