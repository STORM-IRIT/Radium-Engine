#ifndef RADIUMENGINE_TEX3D_HPP_
#define RADIUMENGINE_TEX3D_HPP_

#include <Eigen/Core>

#include <Core/Containers/Grid.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
/// This class stores a discretized N-D function defined inside a N-D
/// bounding box. It evaluates the function at a given point in space
/// wrt the stored values N-linear interpolation.
template <typename T, uint N>
class Tex : public Grid<T, N>
{

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    using IdxVector = typename Grid<T, N>::IdxVector;
    using Vector    = Eigen::Matrix<Scalar, N, 1>;
    using AabbND    = Eigen::AlignedBox<Scalar, N>;

    /// Construct a Tex with the given resolution in the box given by two points.
    Tex( const IdxVector& resolution, const Vector& start, const Vector& end );

    /// Construct a Tex with the give resulution in the given AABB.
    Tex( const IdxVector& resolution, const AabbND& aabb );

    /// Copy constructor and assignment operator perform a deep copy.
    Tex( const Tex& other ) = default;
    Tex& operator=( const Tex& other ) = default;

    inline const AabbND& getAabb() const;

    /// Tri-linear interpolation of the grid values at position v.
    T fetch( const Vector& v ) const;

  private:
    /// The bounding box of the portion of space represented.
    AabbND m_aabb;
    /// The size of a discrete cell
    /// i.e. componentwise division of the aabb extents by the spatial resolution.
    Vector m_cellSize;
};

template <typename T>
using Tex2D = Tex<T, 2>;

template <typename T>
using Tex3D = Tex<T, 3>;
} // namespace Core
} // namespace Ra
#include <Core/Containers/Tex.inl>
#endif // RADIUMENGINE_TEX3D_HPP
