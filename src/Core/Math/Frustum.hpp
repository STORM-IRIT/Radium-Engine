#ifndef RADIUMENGINE_FRUSTUM_HPP
#define RADIUMENGINE_FRUSTUM_HPP

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        struct Frustum
        {
        public:
            enum FACES : int {
                FRONT   = 0,    // Near plane
                BACK    = 1,    // Far plane
                TOP     = 2,
                BOTTOM  = 3,
                LEFT    = 4,
                RIGHT   = 5
            } ;

            RA_CORE_ALIGNED_NEW

            /// Default constructor
            Frustum(const Matrix4 & mvp)
            {
                // Near clipping plane.
                m_planes[FRONT]     = mvp.row(3) + mvp.row(2);
                // Far clipping plane.
                m_planes[BACK]      = mvp.row(3) - mvp.row(2);
                // Top clipping plane.
                m_planes[TOP]       = mvp.row(3) - mvp.row(1);
                // Bottom clipping plane.
                m_planes[BOTTOM]    = mvp.row(3) + mvp.row(1);
                // Left clipping plane.
                m_planes[LEFT]      = mvp.row(3) + mvp.row(0);
                // Right clipping plane.
                m_planes[RIGHT]     = mvp.row(3) - mvp.row(0);
            }

            Vector4 getPlane(uint p) const
            {
                return m_planes[p];
            }

        public:
            /// Clipping planes
            Vector4 m_planes[6];
        };
    }
}

#endif //RADIUMENGINE_RAY_HPP
