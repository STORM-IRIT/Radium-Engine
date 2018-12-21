#ifndef RADIUMENGINE_FRUSTUM_HPP
#define RADIUMENGINE_FRUSTUM_HPP

#include <Core/RaCore.hpp>
#include <Eigen/Core>

namespace Ra {
namespace Core {
namespace Geometry {
struct Frustum {
  public:
    enum FACES : int {
        FRONT = 0, // Near plane
        BACK = 1,  // Far plane
        TOP = 2,
        BOTTOM = 3,
        LEFT = 4,
        RIGHT = 5
    };

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Default constructor
    inline Frustum( const Eigen::Matrix<Scalar, 4, 4>& mvp ) {
        // Near clipping plane.
        m_planes[FRONT] = mvp.row( 3 ) + mvp.row( 2 );
        // Far clipping plane.
        m_planes[BACK] = mvp.row( 3 ) - mvp.row( 2 );
        // Top clipping plane.
        m_planes[TOP] = mvp.row( 3 ) - mvp.row( 1 );
        // Bottom clipping plane.
        m_planes[BOTTOM] = mvp.row( 3 ) + mvp.row( 1 );
        // Left clipping plane.
        m_planes[LEFT] = mvp.row( 3 ) + mvp.row( 0 );
        // Right clipping plane.
        m_planes[RIGHT] = mvp.row( 3 ) - mvp.row( 0 );
    }

    inline const Eigen::Matrix<Scalar, 4, 1>& getPlane( uint p ) const { return m_planes[p]; }

  public:
    /// Clipping planes
    std::array<Eigen::Matrix<Scalar, 4, 1>, 6> m_planes;
};
} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_RAY_HPP
