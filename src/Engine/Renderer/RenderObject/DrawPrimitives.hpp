#ifndef RADIUMENGINE_DRAW_PRIMITIVES_HPP_
#define RADIUMENGINE_DRAW_PRIMITIVES_HPP_
#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Ray.hpp>

namespace Ra
{
    namespace  Engine
    {
        class RenderObject;
        class Component;

        /// A set of convenient functions to instantiate simple displays such as points, lines, etc.
        /// note that objects will be drawn in their entity's local frame.
        /// For "instant" debug drawing, @see DebugDisplay.
        namespace DrawPrimitives
        {
            /// Displays given point shown as the crossing of 3 lines of length 'scale'
            RenderObject* Point(const Component* comp,
                                const Core::Vector3& point,
                                const Core::Color& color, Scalar scale = 0.1f);

            /// Displays given vector shown as an arrow originating from 'start'
            RenderObject* Vector(const Component* comp,
                                 const Core::Vector3& start, const Core::Vector3& v,
                                 const Core::Color& color);

            /// Displays given ray as a straight line.
            RenderObject* Ray(const Component* comp,
                             const Core::Ray& ray,
                             const Core::Color& color);

            /// Displays given triangle ABC, either in wireframe (fill = false)
            /// or filled with the color(fill = true).
            RenderObject* Triangle(const Component* comp,
                                   const Core::Vector3& a, const Core::Vector3& b, const Core::Vector3& c,
                                   const Core::Color& color, bool fill = false);

            /// Displays a normal vector emanating from the given point as a vector arrow
            /// and a normal plane of size 'scale'.
            RenderObject* Normal(const Component* comp,
                                 const Core::Vector3& point, const Core::Vector3& normal,
                                 const Core::Color& color, Scalar scale = 0.1f);

            /// Displays a 3D frame representing the given transform.
            /// Each axis has length 'scale' and are in usual colors (X,Y,Z = red, green blue)
            /// Remainder : the transform will be drawn relative to the component's entity transform.
            RenderObject* Frame( const Component* comp,
                                 const Core::Transform& frameFromEntity,
                                 Scalar scale = 0.1f);
        }

    }
}

#endif // RADIUMENGINE_DRAW_PRIMITIVES_HPP_

