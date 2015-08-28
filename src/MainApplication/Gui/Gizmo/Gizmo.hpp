#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
        class Gizmo
        {
        public:
            Gizmo(const Engine::Component* c, const Core::Transform& t)
                    : m_comp(c), m_transform(t) { }

        protected:
            const Engine::Component* m_comp;
            const Core::Transform& m_transform;
            std::vector<Engine::RenderObject*> m_renderObjects;
            
        };


        class TranslateGizmo : public Gizmo
        {
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(const Engine::Component* c, const Core::Transform& t)
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

                    Core::TriangleMesh cone = Core::MeshUtils::makeCone(cylinderEnd, arrowEnd, arrowFrac );

                    // Merge the cylinder and the cone to create the arrow shape.
                    cylinder.append(cone);

                    Engine::Mesh* mesh = new Engine::Mesh("Gizmo Arrow");
                    mesh->loadGeometry(cylinder);
                }


            }


        };
}


#endif // RADIUMENGINE_GIZMO_HPP_
