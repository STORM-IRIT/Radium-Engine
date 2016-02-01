#ifndef TEX3D_HPP
#define TEX3D_HPP

#include <Core/RaCore.hpp>
#include <Core/Containers/Grid.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        /// This class stores a discretized 3D function defined inside a 3D
        /// bounding box. It evaluates the function at a given point in space
        /// wrt the stored values tri-linear interpolation.
        template<typename T>
        class Tex3D : public Grid<T,3>
        {
        public:
            RA_CORE_ALIGNED_NEW

            /// Construct a Tex3D with the given resolution in the box given by two points.
            Tex3D( const Vector3ui& resolution, const Vector3& start, const Vector3& end );

            /// Construct a Tex3D with the give resulution in the given AABB.
            Tex3D( const Vector3ui& resolution, const Aabb& aabb );

            /// Copy constructor and assignment operator perform a deep copy.
            Tex3D( const Tex3D& other) = default;
            Tex3D& operator=(const Tex3D& other) = default;

            inline const Aabb& getAabb() const;

            /// Tri-linear interpolation of the grid values at position v.
            T fetch( const Vector3& v ) const;

        private:
            /// The bounding box of the portion of space represented.
            Aabb m_aabb;
            /// The size of a discrete cell
            /// i.e. componentwise division of the aabb extents by the spatial resolution.
            Vector3 m_cellSize;
        };

    }
}
#include <Core/Containers/Tex3D.inl>
#endif //RADIUMENGINE_TEX3D_HPP
