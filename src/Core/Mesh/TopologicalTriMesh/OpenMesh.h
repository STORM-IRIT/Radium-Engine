#ifndef TOPOLOGICALMESH_OPENMESH_H
#define TOPOLOGICALMESH_OPENMESH_H

#define RELEASE_EIGEN_PLUGIN_PROTECTION_AGAINST_DIRECT_INCLUSION
#define EIGEN_MATRIX_PLUGIN "Core/Mesh/TopologicalTriMesh/EigenOpenMeshPlugin.h"

#include <Core/RaCore.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

// Need to define openMesh namespace function before including openmesh, for
// MSVC
namespace OpenMesh
{

template <typename Derived>
typename Derived::Scalar dot( Eigen::MatrixBase<Derived> const& v1,
                              Eigen::MatrixBase<Derived> const& v2 )
{
    return v1.dot( v2 );
}

template <typename Derived>
typename Derived::Scalar norm( Eigen::MatrixBase<Derived> const& v1 )
{
    return v1.norm();
}

template <typename Derived>
Derived normalize( Eigen::MatrixBase<Derived>& v1 )
{
    v1.normalize();
    return v1;
}

template <typename Derived>
void vectorize( Eigen::MatrixBase<Derived>& v1, Scalar a )
{
    v1.setConstant( a );
}

template <typename Derived>
typename Derived::Scalar sqrnorm( Eigen::MatrixBase<Derived> const& v1 )
{
    return v1.squaredNorm();
}

template <typename Derived>
typename Derived::template cross_product_return_type<Derived>::type
cross( Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2 )
{
    return v1.cross( v2 );
}

} // namespace OpenMesh

#include <Core/Math/LinearAlgebra.hpp>

#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace OpenMesh
{
template <>
struct vector_traits<Ra::Core::Vector3>
{
    using vector_type = Ra::Core::Vector3;
    using value_type = Scalar;
    static const size_t size_ = 3;
    static size_t size() { return 3; }
};

} // namespace OpenMesh
#endif // TOPOLOGICALMESH_OPENMESH_H
