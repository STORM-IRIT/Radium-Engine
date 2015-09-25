#include <MainApplication/Viewer/Gizmo/RotateGizmo.hpp>

#include <Core/Math/RayCast.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/DebugDisplay/DebugDisplay.hpp>

namespace Ra
{
    namespace Gui
    {
        RotateGizmo::RotateGizmo(Engine::Component* c, const Core::Transform& t, Mode mode)
                : Gizmo(c, t, mode), m_initialPix(Core::Vector2::Zero()), m_selectedAxis(-1)
        {
            constexpr Scalar tubeHeight = 0.001f;
            constexpr Scalar tubeOutRadius = 0.1f;
            constexpr Scalar tubeInRadius = 0.9f * tubeOutRadius;
            // For x,y,z
            for (uint i = 0; i < 3; ++i)
            {
                const Core::Vector3 tubeAxis = Core::Vector3::Unit(i);

                Core::TriangleMesh tube = Core::MeshUtils::makeTube(-tubeHeight*tubeAxis, tubeHeight*tubeAxis, tubeOutRadius, tubeInRadius);

                Core::Color tubeColor= Core::Color::Zero();
                tubeColor[i] = 1.f;
                Core::Vector4Array colors(tube.m_vertices.size(), tubeColor);

                Engine::Mesh* mesh = new Engine::Mesh("Gizmo Arrow");
                mesh->loadGeometry(tube);
                mesh->addData(Engine::Mesh::VERTEX_COLOR, colors);

                Engine::RenderObject* arrowDrawable = new Engine::RenderObject("Gizmo Arrow", m_comp, true);

                Engine::RenderTechnique* rt = new Engine::RenderTechnique;
                rt->shaderConfig = Ra::Engine::ShaderConfiguration("Plain", "../Shaders");
                rt->material = new Ra::Engine::Material("Default material");
                arrowDrawable->setRenderTechnique(rt);
                arrowDrawable->setType(Engine::RenderObject::Type::RO_UI);
                arrowDrawable->setMesh(mesh);

                updateTransform(m_transform);

                m_renderObjects.push_back(m_comp->addRenderObject(arrowDrawable));

            }
        }

        void RotateGizmo::updateTransform(const Core::Transform& t)
        {
            m_transform = t;
            Core::Transform displayTransform = Core::Transform::Identity();
            if (m_mode == LOCAL)
            {
                displayTransform = m_transform;
            }
            else
            {
                displayTransform.translate(m_transform.translation());
            }

            for (auto roIdx : m_renderObjects)
            {
                Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
                        roIdx)->setLocalTransform(displayTransform);
            }
        }

        void RotateGizmo::selectConstraint(int drawableIdx)
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

        Core::Transform RotateGizmo::mouseMove(const Engine::Camera& cam, const Core::Vector2& nextXY)
        {
            if (m_selectedAxis >= 0)
            {

                const Core::Vector3 origin = m_transform.translation();
                Core::Vector3 rotationAxis = Core::Vector3::Unit(m_selectedAxis);
                if (m_mode == LOCAL)
                {
                    rotationAxis = m_transform.rotation()*rotationAxis;
                }

                // Project the clicked points against the plane defined by the rotation circles.
                std::vector<Scalar> hits1, hits2;
                Core::Ray rayToFirstClick  = cam.getRayFromScreen(m_initialPix);
                Core::Ray rayToCurrentClick = cam.getRayFromScreen(nextXY);
                bool hit1 = Core::RayCast::vsPlane(rayToFirstClick, origin, rotationAxis, hits1);
                bool hit2 = Core::RayCast::vsPlane(rayToCurrentClick, origin, rotationAxis, hits2);

                if (hit1 && hit2)
                {
                    // Do the calculations relative to the circle center.
                    const Core::Vector3 originalHit = rayToFirstClick.at(hits1[0]) - origin;
                    const Core::Vector3 currentHit = rayToCurrentClick.at(hits2[0]) - origin;

                    // Get the angle between the two vectors with the correct sign
                    // (since we already know our current rotation axis).
                    auto c = originalHit.cross(currentHit);
                    Scalar d = originalHit.dot(currentHit);

                    Scalar angle = Core::Math::sign(c.dot(rotationAxis)) * std::atan2(c.norm(),d);

                    // Apply rotation.
                    m_transform.prerotate(Core::AngleAxis(angle, rotationAxis));

                }
                m_initialPix = nextXY;
            }
            return m_transform;
        }

        void RotateGizmo::setInitialState(const Engine::Camera& cam, const Core::Vector2& initialXY)
        {
            m_initialPix =  initialXY;
        }
    }
}
