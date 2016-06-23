#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>

#include <Core/Math/RayCast.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
    namespace Gui
    {
        RotateGizmo::RotateGizmo(Engine::Component* c, const Core::Transform &worldTo, const Core::Transform& t, Mode mode)
                : Gizmo(c, worldTo, t, mode), m_initialPix(Core::Vector2::Zero()), m_selectedAxis(-1)
        {
            constexpr Scalar torusOutRadius = 0.1f;
            constexpr Scalar torusAspectRatio = 0.1f;
            // For x,y,z
            for (uint i = 0; i < 3; ++i)
            {
                Core::TriangleMesh torus = Core::MeshUtils::makeParametricTorus<32>(torusOutRadius, torusAspectRatio*torusOutRadius);
                // Transform the torus from z-axis to axis i.
                if (i < 2)
                {
                    for (auto& v: torus.m_vertices)
                    {
                        std::swap( v[2], v[i]);
                    }
                }

                Core::Color torusColor= Core::Color::Zero();
                torusColor[i] = 1.f;
                Core::Vector4Array colors(torus.m_vertices.size(), torusColor);

                std::shared_ptr<Engine::Mesh> mesh( new Engine::Mesh("Gizmo Arrow") );
                mesh->loadGeometry(torus);
                mesh->addData(Engine::Mesh::VERTEX_COLOR, colors);

                Engine::RenderObject* arrowDrawable = new Engine::RenderObject("Gizmo Arrow", m_comp,
                                                                               Engine::RenderObjectType::UI);

                Engine::RenderTechnique* rt = new Engine::RenderTechnique;
                rt->shaderConfig = Ra::Engine::ShaderConfigurationFactory::getConfiguration("Plain");
                rt->material = new Ra::Engine::Material("Default material");
                arrowDrawable->setRenderTechnique(rt);
                arrowDrawable->setMesh( mesh );

                updateTransform(m_worldTo, m_transform);

                m_renderObjects.push_back(m_comp->addRenderObject(arrowDrawable));

            }
        }

        void RotateGizmo::updateTransform(const Core::Transform& worldTo, const Core::Transform& t)
        {
            m_worldTo = worldTo;
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
                        roIdx)->setLocalTransform(m_worldTo * displayTransform);
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
                const Core::Vector3 origin =  m_transform.translation();
                Core::Vector3 rotationAxis = Core::Vector3::Unit(m_selectedAxis);


                // Decompose the current transform's linear part into rotation and scale
                Core::Matrix3 rotationMat;
                Core::Matrix3 scaleMat;
                m_transform.computeRotationScaling(&rotationMat, &scaleMat);

                if (m_mode == LOCAL)
                {
                    rotationAxis = rotationMat*rotationAxis;
                }

                // Project the clicked points against the plane defined by the rotation circles.
                std::vector<Scalar> hits1, hits2;
                Core::Ray rayToFirstClick  = cam.getRayFromScreen(m_initialPix);
                Core::Ray rayToCurrentClick = cam.getRayFromScreen(nextXY);
                bool hit1 = Core::RayCast::vsPlane(rayToFirstClick,   m_worldTo * origin, m_worldTo * rotationAxis, hits1);
                bool hit2 = Core::RayCast::vsPlane(rayToCurrentClick, m_worldTo * origin, m_worldTo * rotationAxis, hits2);

                if (hit1 && hit2)
                {
                    // Do the calculations relative to the circle center.
                    const Core::Vector3 originalHit = rayToFirstClick.pointAt(hits1[0]) - m_worldTo * origin;
                    const Core::Vector3 currentHit = rayToCurrentClick.pointAt(hits2[0]) - m_worldTo * origin;

                    // Get the angle between the two vectors with the correct sign
                    // (since we already know our current rotation axis).
                    auto c = originalHit.cross(currentHit);
                    Scalar d = originalHit.dot(currentHit);

                    Scalar angle = Core::Math::sign(c.dot(m_worldTo * rotationAxis)) * std::atan2(c.norm(),d);

                    // Apply rotation.
                    auto newRot = Core::AngleAxis(angle, rotationAxis) * rotationMat;
                    m_transform.fromPositionOrientationScale(origin, newRot, scaleMat.diagonal() );

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
