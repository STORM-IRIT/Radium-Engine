#ifndef RADIUM_OBB_HPP_
#define RADIUM_OBB_HPP_

#include <Core/RaCore.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace Ra {
namespace Core {
namespace Geometry {
/**
 * An oriented bounding box.
 */
class Obb {
  public:
    using Transform = Eigen::Transform<Scalar, 3, Eigen::Affine>;
    using Aabb = Eigen::AlignedBox<Scalar, 3>;

    /**
     * Initializes an empty bounding box.
     */
    inline Obb() : m_aabb(), m_transform( Transform::Identity() ) {}

    /**
     * Initialize an Obb from an Aabb and a transform.
     */
    inline Obb( const Aabb& aabb, const Transform& tr ) : m_aabb( aabb ), m_transform( tr ) {}

    Obb( const Obb& other ) = default;

    Obb& operator=( const Obb& other ) = default;

    inline ~Obb() {}

    /**
     * Return the Aabb enclosing this.
     */
    inline Aabb toAabb() const {
        Aabb tmp;
        for ( int i = 0; i < 8; ++i )
        {
            tmp.extend( m_transform * m_aabb.corner( static_cast<Aabb::CornerType>( i ) ) );
        }
        return tmp;
    }

    /**
     * Extends the Obb with an new point.
     */
    inline void addPoint( const Eigen::Matrix<Scalar, 3, 1>& p ) { m_aabb.extend( p ); }

    /**
     * Returns the position of the i^th corner of AABB (model space).
     */
    inline Eigen::Matrix<Scalar, 3, 1> corner( int i ) const {
        return m_aabb.corner( static_cast<Aabb::CornerType>( i ) );
    }

    /**
     * Returns the position of the ith corner of the Obb ( world space ).
     */
    inline Eigen::Matrix<Scalar, 3, 1> worldCorner( int i ) const {
        return m_transform * m_aabb.corner( static_cast<Aabb::CornerType>( i ) );
    }

    /**
     * Non-const access to the Obb transformation.
     */
    Transform& transform() { return m_transform; }

    /**
     * Const access to the Obb transformation.
     */
    const Transform& transform() const { return m_transform; }

  private:
    /// The untransformed Aabb.
    Aabb m_aabb;

    /// Orientation of the box.
    Transform m_transform;
};
} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUM_OBB_HPP_
