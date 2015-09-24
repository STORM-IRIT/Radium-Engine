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
            int oldAxis = m_selectedAxis;
            m_selectedAxis = -1;
            if (drawableIdx >= 0)
            {
                auto found = std::find(m_renderObjects.cbegin(), m_renderObjects.cend(), Core::Index(drawableIdx));
                if (found != m_renderObjects.cend())
                {
                    m_selectedAxis = int(found - m_renderObjects.begin());
                }
            }
            if (m_selectedAxis != oldAxis)
            {
                m_initialPix = Core::Vector2::Zero();
            }
        }

        Core::Transform RotateGizmo::mouseMove(const Engine::Camera& cam, const Core::Vector2& nextXY)
        {
            if (m_selectedAxis >= 0)
            {
                // Taken from Rodolphe's View engine gizmos -- see axis_rotation().

                const Core::Vector3 origin = m_transform.translation();
                Core::Vector3 rotationAxis = Core::Vector3::Unit(m_selectedAxis);
                if (m_mode == LOCAL)
                {
                    rotationAxis = m_transform.rotation()*rotationAxis;
                }

                // Raycast vs cirle plane.
                std::vector<Scalar> hits1, hits2;
                Core::Ray rayToFirstClick  = cam.getRayFromScreen(m_initialPix);
                Core::Ray rayToCurrentClick = cam.getRayFromScreen(nextXY);
                bool hit1 = Core::RayCast::vsPlane(rayToFirstClick, origin, rotationAxis, hits1);
                bool hit2 = Core::RayCast::vsPlane(rayToCurrentClick, origin, rotationAxis, hits2);

                if (hit1 && hit2)
                {
                    Core::Vector3 originalHit = rayToFirstClick.at(hits1[0]);
                    Core::Vector3 currentHit = rayToCurrentClick.at(hits2[0]);


                    Core::Vector2 diff = nextXY - m_initialPix;
                    Core::Vector3 tangentDir = 3.0f * rotationAxis.cross(originalHit - origin).normalized();

                    Core::Vector3 p0 = currentHit - tangentDir;
                    Core::Vector3 p1 = currentHit + tangentDir;

                    Core::Vector2 projTangent = cam.project(p1) - cam.project(p0);

                    Scalar dot = projTangent.dot(diff) / 50.f;
                    Scalar angle = 0.01f* (dot > 0 ? 1.f : -1.f) * std::fmod(std::abs(dot), Core::Math::PiMul2);

                    m_mode == LOCAL ?
                    m_transform.rotate(Core::AngleAxis(angle, rotationAxis)):
                    m_transform.prerotate(Core::AngleAxis(angle, rotationAxis));


                }
            }
            return m_transform;
        }

        void RotateGizmo::setInitialState(const Engine::Camera& cam, const Core::Vector2& initialXY)
        {
            //const Core::Vector3 origin = m_transform.translation();
            //const Core::Vector2 orgScreen = cam.project(origin);
            m_initialPix =  initialXY;
        }
    }
}
