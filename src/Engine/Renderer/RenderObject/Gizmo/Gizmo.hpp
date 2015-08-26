#ifndef RADIUMENGINE_GIZMO_HPP_
#define RADIUMENGINE_GIZMO_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
    namespace Engine
    {
        class Gizmo : public RenderObject
        {
        public:
            Gizmo(const Component* c, const Core::Transform& t)
                    : RenderObject("Gizmo", c), m_transform(t) { }

        protected:
            const Core::Transform& m_transform;
        };


        class TranslateGizmo : public Gizmo
        {
            // Draw a frame with : 3 RGB arrows
            TranslateGizmo(const Component* c, const Core::Transform& t)
                    : Gizmo(c, t)
            {
                const Scalar scale = 1.f;
                const Scalar arrowFract = 0.1f;

                Core::Vector3 pos = m_transform.translation();
                Core::Vector3 x = m_transform.linear() * Core::Vector3::UnitX();
                Core::Vector3 y = m_transform.linear() * Core::Vector3::UnitY();
                Core::Vector3 z = m_transform.linear() * Core::Vector3::UnitZ();


                Core::Vector3Array vertices = {
                        // X arrow
                        pos,
                        pos + scale * x,
                        pos + ((1.f - arrowFract) * scale * x) + (arrowFract * scale * y),
                        pos + ((1.f - arrowFract) * scale * x) - (arrowFract * scale * y),

                        // Y arrow
                        pos,
                        pos + scale * y,
                        pos + ((1.f - arrowFract) * scale * y) + (arrowFract * scale * z),
                        pos + ((1.f - arrowFract) * scale * y) - (arrowFract * scale * z),

                        // Z arrow
                        pos,
                        pos + scale * z,
                        pos + ((1.f - arrowFract) * scale * z) + (arrowFract * scale * x),
                        pos + ((1.f - arrowFract) * scale * z) - (arrowFract * scale * x)
                };

                std::vector<uint> indices = { 0, 1, 1, 2,  1, 3,
                                              4, 5, 5, 6,  5, 7,
                                              8, 9, 9, 10, 9, 11};


                Core::Vector4Array colors = {
                        Core::Colors::Red(), Core::Colors::Red(),Core::Colors::Red(), Core::Colors::Red(),
                        Core::Colors::Green(), Core::Colors::Green(),Core::Colors::Green(), Core::Colors::Green(),
                        Core::Colors::Blue(), Core::Colors::Blue(),Core::Colors::Blue(), Core::Colors::Blue(),
                };

                Mesh* mesh = new Mesh();
                mesh->loadGeometry(vertices, indices);
                mesh->addData(Mesh::VERTEX_COLOR, colors);

            }


        };
    }
}


#endif // RADIUMENGINE_GIZMO_HPP_
