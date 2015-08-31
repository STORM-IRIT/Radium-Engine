#include <MainApplication/Gui/Gizmo/Gizmo.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
            TranslateGizmo::TranslateGizmo(const Engine::Component* c, const Core::Transform& t)
            : Gizmo(c, t)
            {

                constexpr Scalar axisWidth = 0.05f;
                constexpr Scalar arrowFrac = 0.1f;

                // For x,y,z
                for (uint i = 0; i < 3; ++i)
                {
                    Core::Vector3 cylinderEnd = Core::Vector3::Zero();
                    Core::Vector3 arrowEnd = Core::Vector3::Zero();
                    cylinderEnd[i] =  (1.f - arrowFrac);
                    arrowEnd[i] = 1.f;

                    Core::TriangleMesh cylinder = Core::MeshUtils::makeCylinder(
                            Core::Vector3::Zero(), cylinderEnd, axisWidth / 2.f );

                    Core::TriangleMesh cone = Core::MeshUtils::makeCone(cylinderEnd, arrowEnd, arrowFrac/2.f );

                    // Merge the cylinder and the cone to create the arrow shape.
                    cylinder.append(cone);


                    Core::Color arrowColor = Core::Color::Zero();
                    arrowColor[i] = 1.f;
                    Core::Vector4Array colors(cylinder.m_vertices.size(), arrowColor);

                    Engine::Mesh* mesh = new Engine::Mesh("Gizmo Arrow");
                    mesh->loadGeometry(cylinder);
                    mesh->addData(Engine::Mesh::VERTEX_COLOR, colors);

                    Engine::RenderObject* arrowDrawable = new Engine::RenderObject("Gizmo Arrow", m_comp);
                    m_renderObjects.push_back(arrowDrawable);

                    Engine::RenderTechnique* rt = new Engine::RenderTechnique;
                    rt->shaderConfig = Ra::Engine::ShaderConfiguration("Plain", "../Shaders");
                    rt->material = new Ra::Engine::Material("Default material");
                    arrowDrawable->setRenderTechnique(rt);
                    arrowDrawable->setMesh(mesh);

                    m_comp->addDrawable(arrowDrawable);

                }


            }



}