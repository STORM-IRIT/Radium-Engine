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
                m_planes[FRONT]     = mvp.row(3) + mvp.row(2) ; //Vector4(mvp(0,3)+mvp(0,2), mvp(1,3)+mvp(1,2), mvp(2,3)+mvp(2,2), mvp(3,3)+mvp(3,2));
                // Far clipping plane.
                m_planes[BACK]      = mvp.row(3) - mvp.row(2) ; //Vector4(mvp(0,3)-mvp(0,2), mvp(1,3)-mvp(1,2), mvp(2,3)-mvp(2,2), mvp(3,3)-mvp(3,2));
                // Top clipping plane.
                m_planes[TOP]       = mvp.row(3) - mvp.row(1) ; //Vector4(mvp(0,3)-mvp(0,1), mvp(1,3)-mvp(1,1), mvp(2,3)-mvp(2,1), mvp(3,3)-mvp(3,1));
                // Bottom clipping plane.
                m_planes[BOTTOM]    = mvp.row(3) + mvp.row(1) ; //Vector4(mvp(0,3)+mvp(0,1), mvp(1,3)+mvp(1,1), mvp(2,3)+mvp(2,1), mvp(3,3)+mvp(3,1));
                // Left clipping plane.
                m_planes[LEFT]      = mvp.row(3) + mvp.row(0) ; //Vector4(mvp(0,3)+mvp(0,0), mvp(1,3)+mvp(1,0), mvp(2,3)+mvp(2,0), mvp(3,3)+mvp(3,0));
                // Right clipping plane.
                m_planes[RIGHT]     = mvp.row(3) - mvp.row(0) ; //Vector4(mvp(0,3)-mvp(0,0), mvp(1,3)-mvp(1,0), mvp(2,3)-mvp(2,0), mvp(3,3)-mvp(3,0));
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
