#ifndef RADIUMENGINE_TEX3D_HPP
#define RADIUMENGINE_TEX3D_HPP

#include <math/Vector.hpp>
#include <containers/Grid.hpp>

namespace Ra
{
    /// This class stores a discretized 3D function defined inside a 3D
    /// bounding box. It evaluates the function at a given point in space
    /// wrt the stored values tri-linear interpolation.
    template<typename T>
    class Tex3D : public Grid<T, 3>
    {
    public:
        // A useful shortcut.
    public:
        Tex3D( const Vector3i& resolution, const Vector3& start, const Vector3& end );
        Tex3D( const Vector3i& resolution, const Aabb& aabb);

        inline const Aabb& getAabb() const;

        /// Tri-linear interpolation of the grid values at position v.
        T fetch(const Vector3& v);

    private:
        /// The bounding box of the portion of space represented.
        Aabb m_aabb;
        /// The size of a discrete cell
        /// i.e. componentwise division of the aabb extents by the spatial resolution.
        Vector3 m_cellSize;
    };

}
#include <containers/Tex3D.inl>
#endif //RADIUMENGINE_TEX3D_HPP
