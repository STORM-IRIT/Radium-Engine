#ifndef RADIUMENGINE_FRUSTUM_HPP
#define RADIUMENGINE_FRUSTUM_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

/// A Frustum represents a clipping volume delimited by 6 planes.
/// This volume usually ressembles a troncated pyramid.
struct Frustum {
  public:
    /// Used to indicate the face to consider.
    enum FACES : int { FRONT = 0, BACK = 1, TOP = 2, BOTTOM = 3, LEFT = 4, RIGHT = 5 };

    RA_CORE_ALIGNED_NEW

    /// Construct a Frustum from a ModelViewProjection matrix.
    Frustum( const Matrix4& mvp ) {
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

    /// Return the \p p-th delimiting plane.
    /// \see FACES for allowed \p p values.
    Vector4 getPlane( uint p ) const { return m_planes[p]; }

  public:
    /// Clipping planes.
    Vector4 m_planes[6];
};
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_RAY_HPP
