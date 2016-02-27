#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
    namespace Engine
    {
        namespace DrawPrimitives
        {
            // Factor common initialization code
            void initRo( RenderObject* ro, const std::shared_ptr<Mesh>& mesh,
                         const Core::Vector3Array& vertices,
                         const std::vector<uint>& indices,
                         const Core::Color& color )
            {
                // Mesh init
                Core::Vector4Array colors(vertices.size(), color);
                mesh->loadGeometry(vertices, indices);
                mesh->addData(Mesh::Vec4Data::VERTEX_COLOR, colors);

                // Ro init
                RenderTechnique* rt = new RenderTechnique;

                rt->shaderConfig = ShaderConfiguration("Plain", "../Shaders");
                rt->material = new Material("Default material");
                ro->setRenderTechnique(rt);
                ro->setMesh( mesh );
            }

            void initLineRo( RenderObject* ro, const std::shared_ptr<Mesh>& mesh,
                             const Core::Vector3Array& vertices,
                             const std::vector<uint>& indices,
                             const Core::Color& color, Scalar lineWidth )
            {
                Core::Vector4Array colors(vertices.size(), color);
                mesh->loadGeometry(vertices, indices);
                mesh->addData(Mesh::Vec4Data::VERTEX_COLOR, colors);

                // Ro init
                if ( nullptr == ro->getRenderTechnique() )
                {
                    RenderTechnique* rt = new Ra::Engine::RenderTechnique;

                    rt->shaderConfig = ShaderConfiguration( "Lines", "../Shaders",
                                                            ShaderConfiguration::DEFAULT_SHADER_PROGRAM );
                    rt->material = new Ra::Engine::Material("Default material");
                    ro->setRenderTechnique(rt);
                }
                ro->setMesh(mesh);
            }


            RenderObject* Point(const Component* comp, const Core::Vector3& point,
                                const Core::Color& color, Scalar scale)
            {
                RenderObject* ro = new Ra::Engine::RenderObject( "Point Primitive", comp, RenderObjectType::Debug );

                Core::Vector3Array vertices = {
                        (point + (scale * Core::Vector3::UnitX())),
                        (point - (scale * Core::Vector3::UnitX())),

                        (point + (scale * Core::Vector3::UnitY())),
                        (point - (scale * Core::Vector3::UnitY())),

                        (point + (scale * Core::Vector3::UnitZ())),
                        (point - (scale * Core::Vector3::UnitZ()))
                };

                std::vector<uint> indices = {0, 1, 2, 3, 4, 5};

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("Point Primitive", GL_LINES) );
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            void Point( RenderObject* ro, const Core::Vector3& point,
                        const Core::Color& color, Scalar scale )
            {
                Core::Vector3Array vertices = {
                        (point + (scale * Core::Vector3::UnitX())),
                        (point - (scale * Core::Vector3::UnitX())),

                        (point + (scale * Core::Vector3::UnitY())),
                        (point - (scale * Core::Vector3::UnitY())),

                        (point + (scale * Core::Vector3::UnitZ())),
                        (point - (scale * Core::Vector3::UnitZ()))
                };

                std::vector<uint> indices = {0, 1, 2, 3, 4, 5};

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* Vector(const Component* comp, const Core::Vector3& start, const Core::Vector3& v,
                                 const Core::Color& color)
            {
                Core::Vector3 end = start+v;
                Core::Vector3 a,b;
                Core::Vector::getOrthogonalVectors(v,a,b);
                a.normalize();
                Scalar l = v.norm();

                const Scalar arrowFract = 0.1f;

                Core::Vector3Array vertices = {
                    start,
                    end,
                    start + ((1.f-arrowFract) * v) + ((arrowFract * l) * a),
                    start + ((1.f-arrowFract) * v) - ((arrowFract * l) * a)
                };
                std::vector<uint> indices = {0,1,  1,2,  1,3};

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("Vector Primitive", GL_LINES) );
                RenderObject* ro = new Ra::Engine::RenderObject("Vector Primitive", comp, RenderObjectType::Debug );
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            void Vector( RenderObject* ro, const Core::Vector3& start,
                         const Core::Vector3& v, const Core::Color& color )
            {
                Core::Vector3 end = start+v;
                Core::Vector3 a,b;
                Core::Vector::getOrthogonalVectors(v,a,b);
                a.normalize();
                Scalar l = v.norm();

                const Scalar arrowFract = 0.1f;

                Core::Vector3Array vertices = {
                    start,
                    end,
                    start + ((1.f-arrowFract) * v) + ((arrowFract * l) * a),
                    start + ((1.f-arrowFract) * v) - ((arrowFract * l) * a)
                };
                std::vector<uint> indices = {0,1,  1,2,  1,3};

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* Ray(const Component* comp, const Core::Ray& ray, const Core::Color& color, int lifetime)
            {
                Core::Vector3 end = ray.at(1000.f);
                Core::Vector3Array vertices = { ray.m_origin, end };
                std::vector<uint> indices = { 0, 1 };

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("Ray Primitive", GL_LINES) );

                RenderObject* ro = new Ra::Engine::RenderObject("Ray Primitive", comp, RenderObjectType::Debug, lifetime);

                initRo(ro, mesh, vertices, indices, color);

                return ro;
            }

            void Ray( RenderObject* ro, const Core::Ray& ray, const Core::Color& color )
            {
                Core::Vector3 end = ray.at(1000.f);
                Core::Vector3Array vertices = { ray.m_origin, end };
                std::vector<uint> indices = { 0, 1 };

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* Triangle(const Component* comp, const Core::Vector3& a, const Core::Vector3& b, const Core::Vector3& c,
                                   const Core::Color& color, bool fill)
            {
                Core::Vector3Array vertices = {a,b,c};
                std::vector<uint> indices;

                if (fill)
                {
                    indices = { 0, 1, 2 };
                }
                else
                {
                    indices = {0, 1, 1, 2, 2, 3};
                }

                GLenum renderType = fill ? GL_TRIANGLES : GL_LINES;

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("Triangle Primitive", renderType ) );
                RenderObject* ro = new Ra::Engine::RenderObject("Triangle Primitive", comp, RenderObjectType::Debug);

                if (fill)
                {
                    initRo(ro, mesh, vertices, indices, color);
                }
                else
                {
                    initLineRo( ro, mesh, vertices, indices, color, 1.0 );
                }
                return ro;
            }

            void Triangle( RenderObject* ro, const Core::Vector3& a, const Core::Vector3& b,
                           const Core::Vector3& c, const Core::Color& color, bool fill )
            {
                Core::Vector3Array vertices = {a,b,c};
                std::vector<uint> indices;

                if (fill)
                {
                    indices = { 0, 1, 2 };
                }
                else
                {
                    indices = {0, 1, 1, 2, 2, 3};
                }

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( fill ? GL_TRIANGLES : GL_LINES );
            }

            RenderObject* Circle(const Component* comp, const Core::Vector3& center, const Core::Vector3& normal,
                                 Scalar radius, uint segments, const Core::Color& color)
            {
                CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

                Core::Vector3Array vertices(segments);
                std::vector<uint> indices(2*segments);

                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(normal, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
                Scalar theta( 0.0 );
                for (uint i = 0; i < segments; ++i)
                {
                    vertices[i] = center + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane );

                    indices.push_back( (segments - 1 + i) % segments );
                    indices.push_back( i );
                    indices.push_back( ( i + 1 ) % segments );
                    indices.push_back( ( i + 2 ) % segments );

                    theta += thetaInc;
                }


                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh( "Circle Primitive", GL_LINES_ADJACENCY ) );
                RenderObject* ro = new Ra::Engine::RenderObject( "Circle Primitive", comp, RenderObjectType::Debug );
                initLineRo( ro, mesh, vertices, indices, color, 5.0 );
                return ro;
            }

            void Circle( RenderObject* ro, const Core::Vector3& center, const Core::Vector3& normal,
                         Scalar radius, uint segments, const Core::Color& color )
            {
                CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

                Core::Vector3Array vertices(segments);
                std::vector<uint> indices(2*segments);

                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(normal, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
                Scalar theta( 0.0 );
                for (uint i = 0; i < segments; ++i)
                {
                    vertices[i] = center + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane );

                    indices.push_back( (segments - 1 + i) % segments );
                    indices.push_back( i );
                    indices.push_back( ( i + 1 ) % segments );
                    indices.push_back( ( i + 2 ) % segments );

                    theta += thetaInc;
                }

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES_ADJACENCY );
            }

            RenderObject* Sphere( const Component* comp, const Core::Vector3& center,
                                  Scalar radius, const Core::Color& color )
            {
                Core::TriangleMesh sphere = Core::MeshUtils::makeGeodesicSphere( radius, 2 );
                std::vector<uint> indices;
                indices.reserve( 3 * sphere.m_triangles.size() );

                for ( const auto& i : sphere.m_triangles )
                {
                    indices.push_back( i.x() );
                    indices.push_back( i.y() );
                    indices.push_back( i.y() );
                    indices.push_back( i.z() );
                }

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh( "Sphere Primitive", GL_LINES ) );
                RenderObject* ro = new Ra::Engine::RenderObject( "Sphere Primitive", comp, RenderObjectType::Debug );

                Core::Transform t;
                t.setIdentity();
                t.translate( center );
                ro->setLocalTransform( t.matrix() );

                initLineRo( ro, mesh, sphere.m_vertices, indices, color, 5.0 );
                return ro;
            }

            void Sphere( RenderObject* ro, const Core::Vector3& center,
                         Scalar radius, const Core::Color& color )
            {
            }

            RenderObject* Disk(const Component* comp, const Core::Vector3& center, const Core::Vector3& normal,
                               Scalar radius, uint segments, const Core::Color& color)
            {
                CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

                uint seg = segments + 1;
                Core::Vector3Array vertices(seg);
                std::vector<uint> indices;

                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(normal, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
                Scalar theta( 0.0 );

                vertices[0] = center;
                indices.push_back( 0 );
                for (uint i = 1; i < seg; ++i)
                {

                    vertices[i] = center + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane );
                    indices.push_back( i );

                    theta += thetaInc;
                }
                indices.push_back( 1 );

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh( "Circle Primitive", GL_TRIANGLE_FAN ) );
                RenderObject* ro = new Ra::Engine::RenderObject( "Circle Primitive", comp, RenderObjectType::Debug );
                initRo( ro, mesh, vertices, indices, color );
                return ro;
            }

            void Disk( RenderObject* ro, const Core::Vector3& center, const Core::Vector3& normal,
                       Scalar radius, uint segments, const Core::Color& color )
            {
                CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

                uint seg = segments + 1;
                Core::Vector3Array vertices(seg);
                std::vector<uint> indices;

                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(normal, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
                Scalar theta( 0.0 );

                vertices[0] = center;
                indices.push_back( 0 );
                for (uint i = 1; i < seg; ++i)
                {

                    vertices[i] = center + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane );
                    indices.push_back( i );

                    theta += thetaInc;
                }
                indices.push_back( 1 );

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_TRIANGLE_FAN );
            }

            RenderObject* Normal(const Component* comp, const Core::Vector3& point, const Core::Vector3& normal,
                const Core::Color& color, Scalar scale)
            {
                // Display an arrow (just like the Vector() function) plus the normal plane.
                Core::Vector3 n = scale * normal.normalized();

                Core::Vector3 end = point+n;
                Core::Vector3 a,b;
                Core::Vector::getOrthogonalVectors(n,a,b);
                a.normalize();
                b.normalize();

                const Scalar arrowFract = 0.1f;

                Core::Vector3Array vertices = {
                    point,
                    end,
                    point + ((1.f-arrowFract) * n) + ((arrowFract * scale) * a),
                    point + ((1.f-arrowFract) * n) - ((arrowFract * scale) * a),

                    point + (scale * a),
                    point + (scale * b),
                    point - (scale * a),
                    point - (scale * b),
                };
                std::vector<uint> indices = {0,1,  1,2,  1,3,
                                             4,5, 5,6, 6,7, 7,4, 4,6,  5,7};

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("Normal Primitive", GL_LINES) );
                RenderObject* ro = new Ra::Engine::RenderObject("Normal Primitive", comp, RenderObjectType::Debug);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            void Normal( RenderObject* ro, const Core::Vector3& point, const Core::Vector3& normal,
                         const Core::Color& color, Scalar scale )
            {
                // Display an arrow (just like the Vector() function) plus the normal plane.
                Core::Vector3 n = scale * normal.normalized();

                Core::Vector3 end = point+n;
                Core::Vector3 a,b;
                Core::Vector::getOrthogonalVectors(n,a,b);
                a.normalize();
                b.normalize();

                const Scalar arrowFract = 0.1f;

                Core::Vector3Array vertices = {
                    point,
                    end,
                    point + ((1.f-arrowFract) * n) + ((arrowFract * scale) * a),
                    point + ((1.f-arrowFract) * n) - ((arrowFract * scale) * a),

                    point + (scale * a),
                    point + (scale * b),
                    point - (scale * a),
                    point - (scale * b),
                };
                std::vector<uint> indices = {
                    0,1, 1,2, 1,3, 4,5, 5,6, 6,7, 7,4, 4,6, 5,7
                };

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* Frame( const Component* comp, const Core::Transform& frameFromEntity,
                                 Scalar scale )
            {
                // Frame is a bit different from the others since there are 3 lines of different colors.
                Core::Vector3 pos = frameFromEntity.translation();
                Core::Vector3 x = frameFromEntity.linear() * Core::Vector3::UnitX();
                Core::Vector3 y = frameFromEntity.linear() * Core::Vector3::UnitY();
                Core::Vector3 z = frameFromEntity.linear() * Core::Vector3::UnitZ();

                Core::Vector3Array vertices = {
                        pos, pos + scale * x,
                        pos, pos + scale * y,
                        pos, pos + scale * z
                };
                std::vector<uint> indices = {
                        0, 0, 1, 1,
                        2, 2, 3, 3,
                        4, 4, 5, 5 };

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh( "Frame Primitive", GL_LINES_ADJACENCY ) );
                RenderObject* ro = new Ra::Engine::RenderObject("Frame Primitive", comp, RenderObjectType::Debug);
                initLineRo(ro, mesh, vertices, indices, Core::Color::Ones(), 1.0);

                Core::Vector4Array colors = {
                        Core::Colors::Red(), Core::Colors::Red(),
                        Core::Colors::Green(), Core::Colors::Green(),
                        Core::Colors::Blue(), Core::Colors::Blue(),
                };
                // Override colors
                mesh->addData(Mesh::VERTEX_COLOR, colors);
                return ro;
            }

            void Frame( RenderObject* ro, const Core::Transform& frameFromEntity, Scalar scale )
            {
                // Frame is a bit different from the others since there are 3 lines of different colors.
                Core::Vector3 pos = frameFromEntity.translation();
                Core::Vector3 x = frameFromEntity.linear() * Core::Vector3::UnitX();
                Core::Vector3 y = frameFromEntity.linear() * Core::Vector3::UnitY();
                Core::Vector3 z = frameFromEntity.linear() * Core::Vector3::UnitZ();

                Core::Vector3Array vertices = {
                        pos, pos + scale * x,
                        pos, pos + scale * y,
                        pos, pos + scale * z
                };
                std::vector<uint> indices = {
                        0, 0, 1, 1,
                        2, 2, 3, 3,
                        4, 4, 5, 5
                };

                Core::Vector4Array colors = {
                        Core::Colors::Red(), Core::Colors::Red(),
                        Core::Colors::Green(), Core::Colors::Green(),
                        Core::Colors::Blue(), Core::Colors::Blue(),
                };

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES_ADJACENCY );
            }

            RenderObject* Grid(const Component* comp, const Core::Vector3& center, const Core::Vector3& x,
                               const Core::Vector3& y, const Core::Color& color, Scalar cellSize, uint res)
            {

                CORE_ASSERT(res > 1, "Grid has to be at least a 2x2 grid.");
                Core::Vector3Array vertices;
                std::vector<uint> indices;

                Scalar halfWidth = (cellSize * res) / 2.f;

                for (uint i = 0 ; i < res+1; ++i)
                {
                    Scalar xStep =  Scalar(i) - Scalar(res) * cellSize / 2.f;
                    vertices.push_back( center - halfWidth * y + xStep * x);
                    vertices.push_back( center + halfWidth * y + xStep * x);
                    indices.push_back(vertices.size()-2);
                    indices.push_back(vertices.size() -1);
                }

                for (uint i = 0 ; i < res+1; ++i)
                {
                    Scalar yStep =  Scalar(i) - Scalar(res) * cellSize / 2.f;
                    vertices.push_back( center - halfWidth * x + yStep * y);
                    vertices.push_back( center + halfWidth * x + yStep * y);
                    indices.push_back(vertices.size()-2);
                    indices.push_back(vertices.size() -1);
                }

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh( "Frame Primitive", GL_LINES ) );
                RenderObject* ro = new Ra::Engine::RenderObject("Frame Primitive", comp, RenderObjectType::Debug );
                initLineRo(ro, mesh, vertices, indices, color, 1.0);
                return ro;
            }

            void Grid( RenderObject* ro, const Core::Vector3& center, const Core::Vector3& x,
                       const Core::Vector3& y, const Core::Color& color, Scalar cellSize, uint res )
            {
                CORE_ASSERT(res > 1, "Grid has to be at least a 2x2 grid.");
                Core::Vector3Array vertices;
                std::vector<uint> indices;

                Scalar halfWidth = (cellSize * res) / 2.f;

                for (uint i = 0 ; i < res+1; ++i)
                {
                    Scalar xStep =  Scalar(i) - Scalar(res) * cellSize / 2.f;
                    vertices.push_back( center - halfWidth * y + xStep * x);
                    vertices.push_back( center + halfWidth * y + xStep * x);
                    indices.push_back(vertices.size()-2);
                    indices.push_back(vertices.size() -1);
                }

                for (uint i = 0 ; i < res+1; ++i)
                {
                    Scalar yStep =  Scalar(i) - Scalar(res) * cellSize / 2.f;
                    vertices.push_back( center - halfWidth * x + yStep * y);
                    vertices.push_back( center + halfWidth * x + yStep * y);
                    indices.push_back(vertices.size()-2);
                    indices.push_back(vertices.size() -1);
                }

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* AABB( const Component *comp, const Core::Aabb &aabb,
                                const Core::Color &color, Scalar scale )
            {
                Core::Vector3Array vertices( 8 );

                for ( uint i = 0; i < 8; ++i )
                {
                    vertices[i] = aabb.corner( static_cast<Core::Aabb::CornerType>( i ) );
                }

                std::vector<uint> indices = {
                    0,1, 1,3, 3,2, 2,0, // Floor
                    0,4, 1,5, 2,6, 3,7, // Links
                    4,5, 5,7, 7,6, 6,4, // Ceil
                };

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("AABB Primitive", GL_LINES) );
                RenderObject* ro = new Ra::Engine::RenderObject("AABB Primitive", comp, RenderObjectType::Debug);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            void AABB( RenderObject *ro, const Core::Aabb &aabb,
                       const Core::Color &color, Scalar scale )
            {
                Core::Vector3Array vertices( 8 );

                for ( uint i = 0; i < 8; ++i )
                {
                    vertices[i] = aabb.corner( static_cast<Core::Aabb::CornerType>( i ) );
                }

                std::vector<uint> indices = {
                    0,1, 1,2, 2,3, 3,0, // Face 1
                    4,5, 5,6, 6,7, 7,4, // Face 2
                    0,4, 1,5, 2,6, 3,7, // Links between faces
                };

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }

            RenderObject* OBB( const Component* comp, const Core::Obb& obb,
                               const Core::Color& color, Scalar scale )
            {
                Core::Vector3Array vertices( 8 );

                for ( uint i = 0; i < 8; ++i )
                {
                    vertices[i] = obb.worldCorner( i );
                }

                std::vector<uint> indices = {
                    0,1, 1,2, 2,3, 3,0, // Floor
                    4,5, 5,6, 6,7, 7,4, // Ceil
                    0,4, 1,5, 2,6, 3,7, // Links
                };

                std::shared_ptr<Mesh> mesh( new Ra::Engine::Mesh("AABB Primitive", GL_LINES) );
                RenderObject* ro = new Ra::Engine::RenderObject("AABB Primitive", comp, RenderObjectType::Debug);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            void OBB( RenderObject* ro, const Core::Obb& obb,
                      const Core::Color& color, Scalar scale )
            {
                Core::Vector3Array vertices( 8 );

                for ( uint i = 0; i < 8; ++i )
                {
                    vertices[i] = obb.worldCorner( i );
                }

                std::vector<uint> indices = {
                    0,1, 1,2, 2,3, 3,0, // Face 1
                    4,5, 5,6, 6,7, 7,4, // Face 2
                    0,4, 1,5, 2,6, 3,7, // Links between faces
                };

                Core::Vector4Array colors( vertices.size(), color );

                ro->getMesh()->loadGeometry( vertices, indices );
                ro->getMesh()->addData( Mesh::VERTEX_COLOR, colors );
                ro->getMesh()->setRenderMode( GL_LINES );
            }
        }
    }
}
