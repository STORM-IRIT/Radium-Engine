#ifndef RADIUMENGINE_FRUSTUM_HPP
#define RADIUMENGINE_FRUSTUM_HPP

#include <Core/RaCore.hpp>
#include <Eigen/Core>

namespace Ra {
namespace Core {
namespace Geometry {
/**
 * A Frustum represents a clipping volume delimited by 6 planes.
 * This volume usually ressembles a troncated pyramid.
 */
struct Frustum {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Used to indicate the face to consider.
     */
    enum FACES : int { FRONT = 0, BACK = 1, TOP = 2, BOTTOM = 3, LEFT = 4, RIGHT = 5 };

    /**
     * Construct a Frustum from a ModelViewProjection matrix.
     */
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

    /**
     * Return the \p p-th delimiting plane.
     */
    inline const Eigen::Matrix<Scalar, 4, 1>& getPlane( FACES p ) const { return m_planes[p]; }

  public:
    /// Clipping planes.
    std::array<Eigen::Matrix<Scalar, 4, 1>, 6> m_planes;
};
} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_RAY_HPP
