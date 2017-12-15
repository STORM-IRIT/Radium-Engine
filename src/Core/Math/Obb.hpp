#ifndef RADIUM_OBB_HPP_
#define RADIUM_OBB_HPP_

#include <Core/RaCore.hpp>

#include <Core/Math/LinearAlgebra.hpp>
namespace Ra 
{
    namespace Core 
    {
        /// An oriented bounding box.
        class Obb
        {
        public:
            /// Constructors and destructor.
            
            /// Initializes an empty bounding box.
            Obb() 
                : m_aabb(),  m_transform( Transform::Identity() ) {}
            
            /// Initialize an OBB from an AABB and a transform.
            Obb( const Aabb& aabb, const Transform& tr ) 
                : m_aabb( aabb ), m_transform( tr ) {}

            /// Default copy constructor and assignment operator.
            Obb( const Obb& other ) = default;
            Obb& operator=( const Obb& other ) = default;

            ~Obb() {}

            /// Return the AABB enclosing this
            Aabb toAabb() const;

            /// Extends the OBB with an new point.
            void addPoint( const Vector3& p );

            /// Returns the position of the i^th corner of AABB (model space)
            Vector3 corner(int i) const;

            /// Returns the position of the ith corner of the OBB ( world space )
            Vector3 worldCorner( int i ) const;

        public:
            /// The untransformed AABB
            Aabb m_aabb;
            /// Orientation of the box.
            Transform m_transform;
        };
    }
}
#include <Core/Math/Obb.inl>

#endif // RADIUM_OBB_HPP_

