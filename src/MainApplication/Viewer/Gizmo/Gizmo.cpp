#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/RayCast.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/DebugDisplay/DebugDisplay.hpp>

namespace Ra
{
    Gizmo::~Gizmo()
    {
        for (auto ro : m_renderObjects)
        {
            m_comp->removeRenderObject(ro);
        }

    }

    TranslateGizmo::TranslateGizmo(Engine::Component* c, const Core::Transform& t)
            : Gizmo(c, t), m_selectedAxis (-1)
    {
        constexpr Scalar arrowScale = 0.2f;
        constexpr Scalar axisWidth = 0.05f;
        constexpr Scalar arrowFrac = 0.1f;

        // For x,y,z
        for (uint i = 0; i < 3; ++i)
        {
            Core::Vector3 cylinderEnd = Core::Vector3::Zero();
            Core::Vector3 arrowEnd = Core::Vector3::Zero();
            cylinderEnd[i] = (1.f - arrowFrac);
            arrowEnd[i] = 1.f;

            Core::TriangleMesh cylinder = Core::MeshUtils::makeCylinder(
                    Core::Vector3::Zero(), arrowScale * cylinderEnd, arrowScale * axisWidth / 2.f);

            Core::TriangleMesh cone = Core::MeshUtils::makeCone(arrowScale * cylinderEnd, arrowScale * arrowEnd,
                                                                arrowScale * arrowFrac / 2.f);

            // Merge the cylinder and the cone to create the arrow shape.
            cylinder.append(cone);


            Core::Color arrowColor = Core::Color::Zero();
            arrowColor[i] = 1.f;
            Core::Vector4Array colors(cylinder.m_vertices.size(), arrowColor);

            Engine::Mesh* mesh = new Engine::Mesh("Gizmo Arrow");
            mesh->loadGeometry(cylinder);
            mesh->addData(Engine::Mesh::VERTEX_COLOR, colors);

            Engine::RenderObject* arrowDrawable = new Engine::RenderObject("Gizmo Arrow", m_comp, true);

            Engine::RenderTechnique* rt = new Engine::RenderTechnique;
            rt->shaderConfig = Ra::Engine::ShaderConfiguration("Plain", "../Shaders");
            rt->material = new Ra::Engine::Material("Default material");
            arrowDrawable->setRenderTechnique(rt);
            arrowDrawable->setType(Engine::RenderObject::Type::RO_UI);
            arrowDrawable->setMesh(mesh);
            arrowDrawable->setLocalTransform(m_transform);

            m_renderObjects.push_back(m_comp->addRenderObject(arrowDrawable));

        }
    }

    void TranslateGizmo::updateTransform(const Core::Transform& t)
    {
        m_transform = t;
        for (auto roIdx : m_renderObjects)
        {
            Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(roIdx)->setLocalTransform(m_transform);
        }
    }

    void TranslateGizmo::selectConstraint( int drawableIdx )
    {
        m_selectedAxis = -1;
        if (drawableIdx >= 0)
        {
            auto found = std::find(m_renderObjects.cbegin(), m_renderObjects.cend(), Core::Index(drawableIdx));
            if (found != m_renderObjects.cend())
            {
                m_selectedAxis = int(found - m_renderObjects.begin());
            }
        }
    }
}
