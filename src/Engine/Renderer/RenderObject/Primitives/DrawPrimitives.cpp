#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

#include <Core/Math/ColorPresets.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra
{
    namespace Engine
    {
        namespace DrawPrimitives
        {
            // Factor common initialization code
            void initRo(RenderObject* ro, Mesh* mesh,
                        const Core::Vector3Array& vertices,
                        const std::vector<uint>& indices,
                        const Core::Color& color)
            {
                // Mesh init
                Core::Vector4Array colors(vertices.size(), color);
                mesh->loadGeometry(vertices, indices);
                mesh->addData(Mesh::VERTEX_COLOR, colors);

                // Ro init
                RenderTechnique* rt = new Ra::Engine::RenderTechnique;
                rt->shaderConfig = Ra::Engine::ShaderConfiguration("Plain", "../Shaders");
                rt->material = new Ra::Engine::Material("Default material");
                ro->setRenderTechnique(rt);
                ro->setMesh(mesh);
            }


            RenderObject* Point(const Component* comp, const Core::Vector3& point,
                                const Core::Color& color, Scalar scale)
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

                RenderObject* ro = new Ra::Engine::RenderObject("Point Primitive", comp);
                Mesh* mesh = new Ra::Engine::Mesh("Point Primitive", GL_LINES);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
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

                Mesh* mesh = new Ra::Engine::Mesh("Vector Primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Vector Primitive", comp);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            RenderObject* Ray(const Component* comp, const Core::Ray& ray, const Core::Color& color)
            {
                Core::Vector3 end = ray.at(std::numeric_limits<Scalar>::max());
                Core::Vector3Array vertices = { ray.m_origin, end };
                std::vector<uint> indices = { 0, 1 };
                Mesh* mesh = new Ra::Engine::Mesh("Ray Primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Ray Primitive", comp);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            RenderObject* Triangle(const Component* comp, const Core::Vector3& a, const Core::Vector3& b, const Core::Vector3& c,
                                   const Core::Color& color, bool fill)
            {
                Core::Vector3Array vertices = {a,b,c};
                std::vector<uint> indices;

                if (fill) {indices = {0,1,2};}
                else { indices = {0,1, 1,2, 2,3};}

                GLenum renderType = fill ? GL_TRIANGLES : GL_LINES;

                Mesh* mesh = new Ra::Engine::Mesh("Triangle Primitive", renderType);
                RenderObject* ro = new Ra::Engine::RenderObject("Triangle Primitive", comp);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }


            RenderObject* Circle(const Component* comp, const Core::Vector3& center, 
                                 Scalar radius, uint segments, const Core::Color& color)
            {
                CORE_ASSERT( segments > 2, "Cannot draw a circle with less than 3 points" );

                const Scalar x0 = center.x();
                const Scalar y0 = center.y();
                const Scalar z0 = center.z();

                Core::Vector3Array vertices(segments);
                std::vector<uint> indices;

                Scalar thetaInc( Core::Math::PiMul2 / Scalar( segments ) );
                Scalar theta( 0.0 );
                for (uint i = 0; i < segments; ++i)
                {
                    Scalar x = radius * std::cos( theta ) + x0;
                    Scalar y = radius * std::cos( theta ) + y0;
                    Scalar z = z0;

                    vertices[i] = Core::Vector3( x, y, z );

                    indices.push_back( i );
                    indices.push_back( ( i + 1 ) % segments );

                    theta += thetaInc;
                }

                Mesh* mesh = new Ra::Engine::Mesh( "Circle Primitive", GL_LINES );
                RenderObject* ro = new Ra::Engine::RenderObject( "Circle Primitive", comp );
                initRo( ro, mesh, vertices, indices, color );
                return ro;
            }

            RenderObject* Disk(const Component* comp, const Core::Vector3& center, 
                               Scalar radius, uint segments, const Core::Color& color)
            {
                CORE_ERROR("Not implemented");
                return nullptr;
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

                Mesh* mesh = new Ra::Engine::Mesh("Normal Primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Normal Primitive", comp);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
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
                std::vector<uint> indices = { 0,1, 2,3, 4,5 };

                Mesh* mesh = new Ra::Engine::Mesh("Frame Primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Frame Primitive", comp);
                initRo(ro, mesh, vertices, indices, Core::Color::Ones());

                Core::Vector4Array colors = {
                        Core::Colors::Red(), Core::Colors::Red(),
                        Core::Colors::Green(), Core::Colors::Green(),
                        Core::Colors::Blue(), Core::Colors::Blue(),
                };
                // Override colors
                mesh->addData(Mesh::VERTEX_COLOR, colors);
                return ro;
            }
        }
    }
}