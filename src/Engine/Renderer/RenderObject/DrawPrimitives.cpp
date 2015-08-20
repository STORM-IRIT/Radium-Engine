#include <Engine/Renderer/RenderObject/DrawPrimitives.hpp>
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

                RenderObject* ro = new Ra::Engine::RenderObject("Point primitive", comp);
                Mesh* mesh = new Ra::Engine::Mesh("Debug Point Lines", GL_LINES);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            }

            RenderObject* Vector(const Component* comp, const Core::Vector3 start, const Core::Vector3 v,
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
                std::vector<uint> indices = { 0,1,  1,2,  1,3};

                Mesh* mesh = new Ra::Engine::Mesh("Vector primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Triangle Primitive", comp);
                initRo(ro, mesh, vertices, indices, color);
                return ro;
            };
            RenderObject* Triangle(const Component* comp, const Core::Vector3 a, const Core::Vector3 b, const Core::Vector3 c,
                                   const Core::Color& color, bool fill)
            {
                Core::Vector3Array vertices = {a,b,c};
                std::vector<uint> indices;

                if (fill) {indices = {0,1,2};}
                else { indices = {0,1, 1,2, 2,3};}

                GLenum renderType = fill ? GL_TRIANGLES : GL_LINES;

                Mesh* mesh = new Ra::Engine::Mesh("Vector primitive", renderType);
                RenderObject* ro = new Ra::Engine::RenderObject("Triangle Primitive", comp);
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

                Mesh* mesh = new Ra::Engine::Mesh("Vector primitive", GL_LINES);
                RenderObject* ro = new Ra::Engine::RenderObject("Triangle Primitive", comp);
                initRo(ro, mesh, vertices, indices, Core::Color::Ones());

                Core::Vector4Array colors = {
                        Core::Color(1,0,0,1), Core::Color(1,0,0,1), // red
                        Core::Color(0,1,0,1), Core::Color(0,1,0,1), // green
                        Core::Color(0,0,1,1), Core::Color(0,0,1,1), // blue
                };
                // Override colors
                mesh->addData(Mesh::VERTEX_COLOR, colors);
                return ro;
            }
        }
    }
}