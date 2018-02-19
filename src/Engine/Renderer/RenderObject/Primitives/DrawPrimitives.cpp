#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>

namespace Ra {
namespace Engine {
namespace DrawPrimitives {
RenderObject* Primitive( Component* component, const MeshPtr& mesh ) {
    ShaderConfiguration config;
    if ( mesh->getRenderMode() == GL_LINES )
    {
        config = ShaderConfigurationFactory::getConfiguration( "Lines" );
    } else
    { config = ShaderConfigurationFactory::getConfiguration( "Plain" ); }

    std::shared_ptr<RenderTechnique> rt( new RenderTechnique );
    rt->setConfiguration( config );
    rt->resetMaterial( new BlinnPhongMaterial( "Default material" ) );

    RenderObject* ro = new RenderObject( mesh->getName(), component, RenderObjectType::Debug );

    ro->setRenderTechnique( rt );
    ro->setMesh( mesh );

    return ro;
}

// TODO(Charly): Factor mesh init code
MeshPtr Point( const Core::Vector3& point, const Core::Color& color, Scalar scale ) {
    Core::Vector3Array vertices = {( point + ( scale * Core::Vector3::UnitX() ) ),
                                   ( point - ( scale * Core::Vector3::UnitX() ) ),

                                   ( point + ( scale * Core::Vector3::UnitY() ) ),
                                   ( point - ( scale * Core::Vector3::UnitY() ) ),

                                   ( point + ( scale * Core::Vector3::UnitZ() ) ),
                                   ( point - ( scale * Core::Vector3::UnitZ() ) )};

    std::vector<uint> indices = {0, 1, 2, 3, 4, 5};

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Point Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Line( const Core::Vector3& a, const Core::Vector3& b, const Core::Color& color ) {
    Core::Vector3Array vertices = {a, b};

    std::vector<uint> indices = {0, 1};

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Line Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Vector( const Core::Vector3& start, const Core::Vector3& v, const Core::Color& color ) {
    Core::Vector3 end = start + v;
    Core::Vector3 a, b;
    Core::Vector::getOrthogonalVectors( v, a, b );
    a.normalize();
    Scalar l = v.norm();

    const Scalar arrowFract = 0.1f;

    Core::Vector3Array vertices = {
        start, end, start + ( ( 1.f - arrowFract ) * v ) + ( ( arrowFract * l ) * a ),
        start + ( ( 1.f - arrowFract ) * v ) - ( ( arrowFract * l ) * a )};
    std::vector<uint> indices = {0, 1, 1, 2, 1, 3};

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Vector Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Ray( const Core::Ray& ray, const Core::Color& color ) {
    Core::Vector3 end = ray.pointAt( 1000.f );

    Core::Vector3Array vertices = {ray.origin(), end};
    std::vector<uint> indices = {0, 1};

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Ray Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Triangle( const Core::Vector3& a, const Core::Vector3& b, const Core::Vector3& c,
                  const Core::Color& color, bool fill ) {
    Core::Vector3Array vertices = {a, b, c};
    std::vector<uint> indices;

    if ( fill )
    {
        indices = {0, 1, 2};
    } else
    { indices = {0, 1, 1, 2, 2, 0}; }

    Mesh::MeshRenderMode renderType = fill ? Mesh::RM_TRIANGLES : Mesh::RM_LINES;

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Triangle Primitive", renderType ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr QuadStrip( const Core::Vector3& a, const Core::Vector3& x, const Core::Vector3& y,
                   uint quads, const Core::Color& color ) {
    Core::Vector3Array vertices( quads * 2 + 2 );
    std::vector<uint> indices( quads * 2 + 2 );

    Core::Vector3 B = a;
    vertices[0] = B;
    vertices[1] = B + x;
    indices[0] = 0;
    indices[1] = 1;
    for ( uint i = 0; i < quads; ++i )
    {
        B += y;
        vertices[2 * i + 2] = B;
        vertices[2 * i + 3] = B + x;
        indices[2 * i + 2] = 2 * i + 2;
        indices[2 * i + 3] = 2 * i + 3;
    }

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Quad Strip Primitive", Mesh::RM_TRIANGLE_STRIP ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Circle( const Core::Vector3& center, const Core::Vector3& normal, Scalar radius,
                uint segments, const Core::Color& color ) {
    CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

    Core::Vector3Array vertices( segments );
    std::vector<uint> indices( segments );

    Core::Vector3 xPlane, yPlane;
    Core::Vector::getOrthogonalVectors( normal, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
    Scalar theta( 0.0 );
    for ( uint i = 0; i < segments; ++i )
    {
        vertices[i] = center + radius * ( std::cos( theta ) * xPlane + std::sin( theta ) * yPlane );
        indices[i] = i;

        theta += thetaInc;
    }

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Circle Primitive", Mesh::RM_LINE_LOOP ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr CircleArc( const Core::Vector3& center, const Core::Vector3& normal, Scalar radius,
                   Scalar angle, uint segments, const Core::Color& color ) {
    Core::Vector3Array vertices( segments );
    std::vector<uint> indices( segments );

    Core::Vector3 xPlane, yPlane;
    Core::Vector::getOrthogonalVectors( normal, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Scalar thetaInc( 2 * angle / Scalar( segments ) );
    Scalar theta( 0.0 );
    for ( uint i = 0; i < segments; ++i )
    {
        vertices[i] = center + radius * ( std::cos( theta ) * xPlane + std::sin( theta ) * yPlane );
        indices[i] = i;

        theta += thetaInc;
    }

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Arc Circle Primitive", Mesh::RM_LINE_STRIP ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Sphere( const Core::Vector3& center, Scalar radius, const Core::Color& color ) {
    Core::TriangleMesh sphere = Core::MeshUtils::makeGeodesicSphere( radius, 2 );

    for ( auto& t : sphere.vertices() )
    {
        t += center;
    }

    Core::Vector4Array colors( sphere.vertices().size(), color );

    MeshPtr mesh( new Mesh( "Sphere Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( sphere );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Capsule( const Core::Vector3& p1, const Core::Vector3& p2, Scalar radius,
                 const Core::Color& color ) {
    const Scalar l = ( p2 - p1 ).norm();

    Core::TriangleMesh capsule = Core::MeshUtils::makeCapsule( l, radius );

    // Compute the transform so that
    // (0,0,-l/2) maps to p1 and (0,0,l/2) maps to p2

    const Core::Vector3 trans = 0.5f * ( p2 + p1 );
    Core::Quaternion rot =
        Core::Quaternion::FromTwoVectors( Core::Vector3{0, 0, l / 2}, 0.5f * ( p1 - p2 ) );

    Core::Transform t = Core::Transform::Identity();
    t.rotate( rot );
    t.pretranslate( trans );

    for ( auto& v : capsule.vertices() )
    {
        v = t * v;
    }

    Core::Vector4Array colors( capsule.vertices().size(), color );

    MeshPtr mesh( new Mesh( "Sphere Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( capsule );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Disk( const Core::Vector3& center, const Core::Vector3& normal, Scalar radius,
              uint segments, const Core::Color& color ) {
    CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

    uint seg = segments + 1;
    Core::Vector3Array vertices( seg );
    std::vector<uint> indices( seg + 1 );

    Core::Vector3 xPlane, yPlane;
    Core::Vector::getOrthogonalVectors( normal, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
    Scalar theta( 0.0 );

    vertices[0] = center;
    indices[0] = 0;
    for ( uint i = 1; i < seg; ++i )
    {
        vertices[i] = center + radius * ( std::cos( theta ) * xPlane + std::sin( theta ) * yPlane );
        indices[i] = i;

        theta += thetaInc;
    }
    indices[seg] = 1;

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Disk Primitive", Mesh::RM_TRIANGLE_FAN ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Normal( const Core::Vector3& point, const Core::Vector3& normal, const Core::Color& color,
                Scalar scale ) {
    // Display an arrow (just like the Vector() function)
    // plus the normal plane.
    Core::Vector3 n = scale * normal.normalized();

    Core::Vector3 end = point + n;
    Core::Vector3 a, b;
    Core::Vector::getOrthogonalVectors( n, a, b );
    a.normalize();
    b.normalize();

    const Scalar arrowFract = 0.1f;

    Core::Vector3Array vertices = {
        point,
        end,
        point + ( ( 1.f - arrowFract ) * n ) + ( ( arrowFract * scale ) * a ),
        point + ( ( 1.f - arrowFract ) * n ) - ( ( arrowFract * scale ) * a ),

        point + ( scale * a ),
        point + ( scale * b ),
        point - ( scale * a ),
        point - ( scale * b ),
    };

    Core::Vector4Array colors( vertices.size(), color );

    std::vector<uint> indices = {0, 1, 1, 2, 1, 3, 4, 5, 5, 6, 6, 7, 7, 4, 4, 6, 5, 7};

    MeshPtr mesh( new Mesh( "Normal Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Frame( const Core::Transform& frameFromEntity, Scalar scale ) {
    // Frame is a bit different from the others
    // since there are 3 lines of different colors.
    Core::Vector3 pos = frameFromEntity.translation();
    Core::Vector3 x = frameFromEntity.linear() * Core::Vector3::UnitX();
    Core::Vector3 y = frameFromEntity.linear() * Core::Vector3::UnitY();
    Core::Vector3 z = frameFromEntity.linear() * Core::Vector3::UnitZ();

    Core::Vector3Array vertices = {pos, pos + scale * x, pos, pos + scale * y,
                                   pos, pos + scale * z};

    std::vector<uint> indices = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};

    Core::Vector4Array colors = {
        Core::Colors::Red(),   Core::Colors::Red(),  Core::Colors::Green(),
        Core::Colors::Green(), Core::Colors::Blue(), Core::Colors::Blue(),
    };

    MeshPtr mesh( new Mesh( "Frame Primitive", Mesh::RM_LINES_ADJACENCY ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Grid( const Core::Vector3& center, const Core::Vector3& x, const Core::Vector3& y,
              const Core::Color& color, Scalar cellSize, uint res ) {

    CORE_ASSERT( res > 1, "Grid has to be at least a 2x2 grid." );
    Core::Vector3Array vertices;
    std::vector<uint> indices;

    Scalar halfWidth = ( cellSize * res ) / 2.f;

    for ( uint i = 0; i < res + 1; ++i )
    {
        Scalar xStep = Scalar( i ) - Scalar( res ) * cellSize / 2.f;
        vertices.push_back( center - halfWidth * y + xStep * x );
        vertices.push_back( center + halfWidth * y + xStep * x );
        indices.push_back( vertices.size() - 2 );
        indices.push_back( vertices.size() - 1 );
    }

    for ( uint i = 0; i < res + 1; ++i )
    {
        Scalar yStep = Scalar( i ) - Scalar( res ) * cellSize / 2.f;
        vertices.push_back( center - halfWidth * x + yStep * y );
        vertices.push_back( center + halfWidth * x + yStep * y );
        indices.push_back( vertices.size() - 2 );
        indices.push_back( vertices.size() - 1 );
    }

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "GridPrimitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr AABB( const Core::Aabb& aabb, const Core::Color& color ) {
    Core::Vector3Array vertices( 8 );

    for ( uint i = 0; i < 8; ++i )
    {
        vertices[i] = aabb.corner( static_cast<Core::Aabb::CornerType>( i ) );
    }

    std::vector<uint> indices = {
        0, 1, 1, 3, 3, 2, 2, 0, // Floor
        0, 4, 1, 5, 2, 6, 3, 7, // Links
        4, 5, 5, 7, 7, 6, 6, 4, // Ceil
    };

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "AABB Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr OBB( const Core::Obb& obb, const Core::Color& color ) {
    Core::Vector3Array vertices( 8 );

    for ( uint i = 0; i < 8; ++i )
    {
        vertices[i] = obb.worldCorner( i );
    }

    std::vector<uint> indices = {
        0, 1, 1, 3, 3, 2, 2, 0, // Floor
        4, 5, 5, 7, 7, 6, 6, 4, // Ceil
        0, 4, 1, 5, 2, 6, 3, 7, // Links
    };

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "OBB Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}

MeshPtr Spline( const Core::Spline<3, 3>& spline, uint pointCount, const Core::Color& color,
                Scalar scale ) {
    Core::Vector3Array vertices;
    vertices.reserve( pointCount );

    std::vector<uint> indices;
    indices.reserve( pointCount * 2 - 2 );

    Scalar dt = Scalar( 1 ) / Scalar( pointCount - 1 );
    for ( uint i = 0; i < pointCount; ++i )
    {
        Scalar t = dt * i;
        vertices.push_back( spline.f( t ) );
    }

    for ( uint i = 0; i < pointCount - 1; ++i )
    {
        indices.push_back( i );
        indices.push_back( i + 1 );
    }

    Core::Vector4Array colors( vertices.size(), color );

    MeshPtr mesh( new Mesh( "Spline Primitive", Mesh::RM_LINES ) );
    mesh->loadGeometry( vertices, indices );
    mesh->addData( Mesh::VERTEX_COLOR, colors );

    return mesh;
}
} // namespace DrawPrimitives
} // namespace Engine
} // namespace Ra
