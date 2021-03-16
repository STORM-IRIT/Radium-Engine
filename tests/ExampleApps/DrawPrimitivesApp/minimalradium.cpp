
#include <minimalradium.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/LambertianMaterial.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>

#ifdef IO_USE_ASSIMP
#    include <IO/AssimpLoader/AssimpFileLoader.hpp>
#endif

#include <random>

const bool ENABLE_GRID      = true;
const bool ENABLE_CUBES     = true;
const bool ENABLE_POINTS    = true;
const bool ENABLE_LINES     = true;
const bool ENABLE_VECTORS   = true;
const bool ENABLE_RAYS      = true;
const bool ENABLE_TRIANGLES = true;
const bool ENABLE_CIRCLES   = true;
const bool ENABLE_ARCS      = true;
const bool ENABLE_SPHERES   = true;
const bool ENABLE_CAPSULES  = true;
const bool ENABLE_DISKS     = true;
const bool ENABLE_NORMALS   = true;
const bool ENABLE_POLYS     = true;
const bool ENABLE_LOGO      = true;

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Engine;
using namespace Ra::Engine::Rendering;
using namespace Ra::Engine::Data;
using namespace Ra::Engine::Scene;

/**
 * This file contains a minimal radium/qt application which shows the geometrical primitives
 * supported by Radium
 */

MinimalComponent::MinimalComponent( Ra::Engine::Scene::Entity* entity ) :
    Ra::Engine::Scene::Component( "Minimal Component", entity ) {}

void updateCellCorner( Vector3& cellCorner, const Scalar cellSize, const int nCellX, const int ) {

    cellCorner[0] += cellSize;
    if ( cellCorner[0] > cellSize * ( ( 2 * nCellX - 1 ) / 4_ra ) )
    {
        cellCorner[0] = -nCellX * cellSize / 2_ra;
        cellCorner[2] += cellSize;
    }
}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {
    auto blinnPhongMaterial              = make_shared<BlinnPhongMaterial>( "Shaded Material" );
    blinnPhongMaterial->m_perVertexColor = true;
    blinnPhongMaterial->m_ks             = Utils::Color::White();
    blinnPhongMaterial->m_ns             = 100_ra;

    auto plainMaterial              = make_shared<PlainMaterial>( "Plain Material" );
    plainMaterial->m_perVertexColor = true;

    auto lambertianMaterial              = make_shared<LambertianMaterial>( "Lambertian Material" );
    lambertianMaterial->m_perVertexColor = true;

    //// setup ////
    Scalar colorBoost = 1_ra; /// since simple primitive are ambient only, boost their color
    Scalar cellSize   = 0.35_ra;
    int nCellX        = 7;
    int nCellY        = 7;
    Vector3 cellCorner {-nCellX * cellSize / 2_ra, 0_ra, -nCellY * cellSize / 2_ra};
    Vector3 toCellCenter {cellSize / 2_ra, cellSize / 2_ra, cellSize / 2_ra};
    Scalar offset {0.05_ra};
    Vector3 offsetVec {offset, offset, offset};
    std::random_device rd;    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen( rd() ); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<Scalar> dis015( 0_ra, cellSize - 2_ra * offset );
    std::uniform_real_distribution<Scalar> dis01( 0_ra, 1_ra );
    std::uniform_real_distribution<Scalar> dis11( -1_ra, 1_ra );
    std::uniform_int_distribution<uint> disInt( 0, 128 );
    uint circleGridSize = 8;
    uint numberOfSphere = 32;

    //// GRID ////
    if ( ENABLE_GRID )
    {

        auto gridPrimitive = DrawPrimitives::Grid( Vector3::Zero(),
                                                   Vector3::UnitX(),
                                                   Vector3::UnitZ(),
                                                   Utils::Color::Grey( 0.6f ),
                                                   cellSize,
                                                   nCellX );

        auto gridRo = RenderObject::createRenderObject(
            "test_grid", this, RenderObjectType::Geometry, gridPrimitive, {} );
        gridRo->setMaterial( Ra::Core::make_shared<PlainMaterial>( "Grid material" ) );
        gridRo->setPickable( false );
        addRenderObject( gridRo );
    }

    //// CUBES ////
    if ( ENABLE_CUBES )
    {
        std::shared_ptr<Mesh> cube1( new Mesh( "Cube" ) );
        auto coord = cellSize / 8_ra;
        cube1->loadGeometry( Geometry::makeSharpBox( Vector3 {coord, coord, coord} ) );
        cube1->getCoreGeometry().addAttrib(
            "in_color", Vector4Array {cube1->getNumVertices(), Utils::Color::Green()} );

        auto renderObject1 = RenderObject::createRenderObject(
            "Cube1", this, RenderObjectType::Geometry, cube1, {} );
        renderObject1->setLocalTransform( Transform {Translation( cellCorner )} );
        renderObject1->setMaterial( blinnPhongMaterial );
        addRenderObject( renderObject1 );

        // another cube
        std::shared_ptr<Mesh> cube2( new Mesh( "Cube" ) );
        coord = cellSize / 4_ra;
        cube2->loadGeometry( Geometry::makeSharpBox( Vector3 {coord, coord, coord} ) );
        cube2->getCoreGeometry().addAttrib(
            "colour", Vector4Array {cube2->getNumVertices(), Utils::Color::Red()} );

        cube2->setAttribNameCorrespondance( "colour", "in_color" );
        auto renderObject2 = RenderObject::createRenderObject(
            "CubeRO_2", this, RenderObjectType::Geometry, cube2, {} );
        coord = cellSize / 2_ra;
        renderObject2->setLocalTransform(
            Transform {Translation( cellCorner + Vector3( coord, coord, coord ) )} );
        renderObject2->setMaterial( lambertianMaterial );
        addRenderObject( renderObject2 );
    }
    //// POINTS ////
    if ( ENABLE_POINTS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto testpoint = RenderObject::createRenderObject(
            "test_point",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Point( cellCorner, colorBoost * Utils::Color {0_ra, 1_ra, 0.3_ra} ),
            {} );
        testpoint->setMaterial( plainMaterial );
        addRenderObject( testpoint );

        for ( int i = 0; i < 10; ++i )
        {
            Vector3 randomVec {cellCorner + offsetVec +
                               Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
            auto point = RenderObject::createRenderObject(
                "test_point",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Point( randomVec, colorBoost * randomCol, 0.03_ra ),
                {} );
            point->setMaterial( plainMaterial );
            addRenderObject( point );
        }
    }
    //// LINES ////
    if ( ENABLE_LINES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        auto testline = RenderObject::createRenderObject(
            "test_line",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Line( cellCorner,
                                  cellCorner + Vector3 {0_ra, 0.4_ra, 0_ra},
                                  colorBoost * Utils::Color::Red() ),
            {} );
        testline->setMaterial( plainMaterial );
        addRenderObject( testline );

        for ( int i = 0; i < 20; ++i )
        {
            Vector3 randomVec1 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Vector3 randomVec2 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

            auto line = RenderObject::createRenderObject(
                "test_line",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Line( randomVec1, randomVec2, colorBoost * randomCol ),
                {} );
            line->setMaterial( plainMaterial );
            addRenderObject( line );
        }
    }
    //// VECTOR ////
    if ( ENABLE_VECTORS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto testvector = RenderObject::createRenderObject(
            "test_vector",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Vector(
                cellCorner, Vector3 {0_ra, 0.5_ra, 0_ra}, colorBoost * Utils::Color::Blue() ),
            {} );
        testvector->setMaterial( plainMaterial );
        addRenderObject( testvector );

        for ( int i = 0; i < 10; ++i )
        {
            Vector3 randomVec1 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Vector3 randomVec2 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

            auto vector = RenderObject::createRenderObject(
                "test_vector",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Vector(
                    randomVec1, randomVec2 - randomVec1, colorBoost * randomCol ),
                {} );
            vector->setMaterial( plainMaterial );
            addRenderObject( vector );
        }
    }

    if ( ENABLE_RAYS )
    {
        /// RAY ////
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto testray = RenderObject::createRenderObject(
            "test_ray",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Ray(
                {cellCorner, {0_ra, 1_ra, 0_ra}}, colorBoost * Utils::Color::Yellow(), cellSize ),
            {} );
        testray->setMaterial( plainMaterial );
        addRenderObject( testray );
    }

    //// TRIANGLES ////
    if ( ENABLE_TRIANGLES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto triangle1 = RenderObject::createRenderObject(
            "test_triangle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle( cellCorner + 4_ra * Vector3 {-0.01_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + 4_ra * Vector3 {+0.01_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + 4_ra * Vector3 {+0.0_ra, 0.02_ra, 0.0_ra},
                                      colorBoost * Utils::Color::White(),
                                      true ),
            {} );
        triangle1->setMaterial( plainMaterial );
        addRenderObject( triangle1 );

        auto triangle2 = RenderObject::createRenderObject(
            "test_triangle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle( cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {-0.071_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {+0.071_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {+0.0_ra, 0.2_ra, 0.0_ra},
                                      colorBoost * Utils::Color::Green(),
                                      true ),
            {} );
        triangle2->setMaterial( plainMaterial );
        addRenderObject( triangle2 );

        for ( int i = 0; i < 10; ++i )
        {
            auto triwire = RenderObject::createRenderObject(
                "test_triangle_wire",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Triangle(
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {-0.071_ra, 0.0_ra, Scalar( i ) / 20_ra * cellSize},
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {+0.071_ra, 0.0_ra, Scalar( i ) / 20_ra * cellSize},
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {+0.0_ra, 0.2_ra, Scalar( i ) / 20_ra * cellSize},
                    colorBoost * Utils::Color::White() * Scalar( i ) / 10_ra,
                    false ),
                {} );
            triwire->setMaterial( plainMaterial );
            addRenderObject( triwire );
        }
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
    if ( ENABLE_CIRCLES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        {
            auto circle = RenderObject::createRenderObject(
                "test_circle",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Circle( cellCorner,
                                        {0_ra, 0_ra, 1_ra},
                                        cellSize / 8_ra,
                                        64,
                                        colorBoost * Utils::Color::White() ),
                {} );
            circle->setMaterial( plainMaterial );
            addRenderObject( circle );
        }
        for ( uint j = 0; j < circleGridSize; ++j )
            for ( uint i = 0; i < circleGridSize; ++i )
            {
                Vector3 circleCenter {
                    cellCorner + offsetVec +
                    Vector3 {Scalar( j ) / circleGridSize * ( cellSize - 2 * offset ),
                             offset,
                             Scalar( i ) / circleGridSize * ( cellSize - 2 * offset )}};
                Vector3 circleNormal {Vector3 {Scalar( i ), Scalar( j ), 10_ra}};
                circleNormal.normalize();
                Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
                Scalar circleRadius {Scalar( circleGridSize / 2 + i ) /
                                     Scalar( 2 * circleGridSize ) * cellSize / 8_ra};
                uint circleSubdiv {3 + j * circleGridSize + i};

                auto circle = RenderObject::createRenderObject(
                    "test_circle",
                    this,
                    RenderObjectType::Geometry,
                    DrawPrimitives::Circle( circleCenter,
                                            circleNormal,
                                            circleRadius,
                                            circleSubdiv,
                                            colorBoost * randomCol ),
                    {} );
                circle->setMaterial( plainMaterial );
                addRenderObject( circle );
            }
    }
    //// CIRCLE ARC ////
    if ( ENABLE_ARCS )
    {
        {
            auto arc = RenderObject::createRenderObject(
                "test_circle",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::CircleArc( cellCorner + Vector3 {0_ra, 2_ra * offset, 0_ra},
                                           {0_ra, 0_ra, 1_ra},
                                           cellSize / 8_ra,
                                           1_ra,
                                           64,
                                           colorBoost * Utils::Color::White() ),
                {} );
            arc->setMaterial( plainMaterial );
            addRenderObject( arc );
        }
        for ( uint j = 0; j < circleGridSize; ++j )
        {
            for ( uint i = 0; i < circleGridSize; ++i )
            {
                Vector3 circleCenter {
                    cellCorner + offsetVec +
                    Vector3 {Scalar( j ) / circleGridSize * ( cellSize - 2 * offset ),
                             2 * offset,
                             Scalar( i ) / circleGridSize * ( cellSize - 2 * offset )}};
                Vector3 circleNormal {0_ra, 0_ra, 1_ra};
                circleNormal.normalize();
                Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
                Scalar circleRadius {( cellSize - 2_ra * offset ) / 20_ra};
                Scalar circleArc {Scalar( i ) / Scalar( circleGridSize ) * 2_ra};
                uint circleSubdiv {2 + j};

                auto arc = RenderObject::createRenderObject(
                    "test_circle",
                    this,
                    RenderObjectType::Geometry,
                    DrawPrimitives::CircleArc( circleCenter,
                                               circleNormal,
                                               circleRadius,
                                               circleArc,
                                               circleSubdiv,
                                               colorBoost * randomCol ),
                    {} );
                arc->setMaterial( plainMaterial );
                addRenderObject( arc );
            }
        }
    }
    //// SPHERE /////

    if ( ENABLE_SPHERES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        {
            auto sphere = RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( cellCorner, 0.02_ra, Utils::Color::White() ),
                {} );
            sphere->setMaterial( blinnPhongMaterial );
            addRenderObject( sphere );
        }
        numberOfSphere = 32;
        for ( uint i = 0; i < numberOfSphere; ++i )
        {
            Scalar angle {Scalar( i ) / Scalar( numberOfSphere ) * 7_ra};
            Scalar ratio {Scalar( i ) / Scalar( numberOfSphere - 1 )};
            Vector3 center {cellCorner + Vector3 {cellSize / 2_ra, ratio * .1_ra, cellSize / 2_ra}};
            Vector3 center1 {center + Vector3 {ratio * cellSize * .4_ra * std::cos( angle ),
                                               0_ra,
                                               ratio * cellSize * .4_ra * std::sin( angle )}};
            Vector3 center2 {
                center +
                Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 2_ra ),
                         0_ra,
                         ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 2_ra )}};

            Vector3 center3 {
                center +
                Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 4_ra ),
                         0_ra,
                         ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 4_ra )}};

            Color color1 {Utils::Color::Green() * ratio};
            Color color2 {Utils::Color::Red() * ratio};
            Color color3 {Utils::Color::Blue() * ratio};
            auto sphere = RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center1, 0.005_ra + ratio * 0.01_ra, color1 ),
                {} );
            sphere->setMaterial( blinnPhongMaterial );
            addRenderObject( sphere );

            sphere = RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center2, 0.005_ra + ratio * 0.01_ra, color2 ),
                {} );
            sphere->setMaterial( blinnPhongMaterial );
            addRenderObject( sphere );

            sphere = RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center3, 0.01_ra + ratio * 0.01_ra, color3 ),
                {} );
            sphere->setMaterial( blinnPhongMaterial );
            addRenderObject( sphere );
        }
    }
    //// CAPSULE ////
    if ( ENABLE_CAPSULES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto capsule = RenderObject::createRenderObject(
            "test_capsule",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Capsule( cellCorner,
                                     cellCorner + Vector3 {0_ra, 0.1_ra, 0_ra},
                                     0.02_ra,
                                     Utils::Color::White() ),
            {} );
        capsule->setMaterial( blinnPhongMaterial );
        addRenderObject( capsule );
    }

    //// DISK ////
    if ( ENABLE_DISKS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto disk = RenderObject::createRenderObject(
            "test_disk",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Disk( cellCorner,
                                  Vector3 {0_ra, 0_ra, 1_ra},
                                  0.05_ra,
                                  32,
                                  colorBoost * Utils::Color::White() ),
            {} );
        disk->setMaterial( blinnPhongMaterial );
        addRenderObject( disk );
    }

    /// NORMAL
    if ( ENABLE_NORMALS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        auto normal = RenderObject::createRenderObject(
            "test_normal",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Normal( cellCorner + Vector3 {0_ra, 0.1_ra, 0_ra},
                                    Vector3 {0_ra, 0_ra, 1_ra},
                                    colorBoost * Utils::Color::White(),
                                    0.01_ra ),
            {} );
        normal->setMaterial( plainMaterial );
        addRenderObject( normal );
    }
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
                              k                     const Core::Utils::Color& color,
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

    //// PolyMesh ////
    if ( ENABLE_POLYS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        Ra::Core::Geometry::PolyMesh polyMesh;
        polyMesh.setVertices( {
            // quad
            {-1.1_ra, -0_ra, 0_ra},
            {1.1_ra, -0_ra, 0_ra},
            {1_ra, 1_ra, 0_ra},
            {-1_ra, 1_ra, 0_ra},
            // hepta
            {2_ra, 2_ra, 0_ra},
            {2_ra, 3_ra, 0_ra},
            {0_ra, 4_ra, 0_ra},
            {-2_ra, 3_ra, 0_ra},
            {-2_ra, 2_ra, 0_ra},
        } );

        Vector3Array normals;
        normals.resize( polyMesh.vertices().size() );
        std::transform(
            polyMesh.vertices().cbegin(),
            polyMesh.vertices().cend(),
            normals.begin(),
            []( const Vector3& v ) { return ( v + Vector3( 0_ra, 0_ra, 1_ra ) ).normalized(); } );
        polyMesh.setNormals( normals );

        auto quad = VectorNui( 4 );
        quad << 0, 1, 2, 3;
        auto hepta = VectorNui( 7 );
        hepta << 3, 2, 4, 5, 6, 7, 8;
        polyMesh.setIndices( {quad, hepta} );

        std::shared_ptr<PolyMesh> poly1( new PolyMesh( "Poly", std::move( polyMesh ) ) );
        poly1->getCoreGeometry().addAttrib(
            "in_color",
            Vector4Array {poly1->getNumVertices(),
                          colorBoost * Utils::Color {1_ra, 0.6_ra, 0.1_ra}} );

        auto renderObject1 = RenderObject::createRenderObject(
            "PolyMesh", this, RenderObjectType::Geometry, poly1, {} );
        renderObject1->setMaterial( blinnPhongMaterial );
        renderObject1->setLocalTransform( Transform {Translation( Vector3( cellCorner ) ) *
                                                     Eigen::UniformScaling<Scalar>( 0.06_ra )} );

        addRenderObject( renderObject1 );
    }

    if ( ENABLE_LOGO )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        Asset::FileData* data;
        auto l               = IO::AssimpFileLoader();
        auto rp              = Resources::getResourcesPath();
        std::string filename = *rp + "/Assets/radium-logo.dae";
        data                 = l.loadFile( filename );
        if ( data != nullptr )
        {
            auto geomData = data->getGeometryData();

            for ( const auto& gd : geomData )
            {
                std::shared_ptr<AttribArrayDisplayable> mesh {nullptr};
                switch ( gd->getType() )
                {
                case Ra::Core::Asset::GeometryData::TRI_MESH:
                    mesh = std::shared_ptr<Mesh> {
                        createMeshFromGeometryData<Geometry::TriangleMesh>( "logo", gd )};
                    break;
                case Ra::Core::Asset::GeometryData::QUAD_MESH:
                case Ra::Core::Asset::GeometryData::POLY_MESH:
                    mesh = std::shared_ptr<PolyMesh> {
                        createMeshFromGeometryData<Geometry::PolyMesh>( "logo", gd )};
                    break;
                default:
                    break;
                }

                std::shared_ptr<Data::Material> roMaterial;
                const Core::Asset::MaterialData* md =
                    gd->hasMaterial() ? &( gd->getMaterial() ) : nullptr;
                // First extract the material from asset or create a default one
                if ( md != nullptr )
                {
                    auto converter =
                        Data::EngineMaterialConverters::getMaterialConverter( md->getType() );
                    auto mat = converter.second( md );
                    roMaterial.reset( mat );
                }
                else
                {
                    auto mat             = new Data::BlinnPhongMaterial( "_DefaultBPMaterial" );
                    mat->m_renderAsSplat = mesh->getNumFaces() == 0;
                    roMaterial.reset( mat );
                }

                // Create the RenderObject
                auto renderObject = RenderObject::createRenderObject(
                    "logo", this, RenderObjectType::Geometry, mesh, {} );
                renderObject->setLocalTransform(
                    Transform {Translation( Vector3( cellCorner ) + toCellCenter ) *
                               Eigen::UniformScaling<Scalar>( cellSize * 0.02_ra )} );
                renderObject->setMaterial( roMaterial );

                addRenderObject( renderObject );
            }
        }
    }
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue*, const Ra::Engine::FrameInfo& ) {}

void MinimalSystem::addComponent( Ra::Engine::Scene::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
