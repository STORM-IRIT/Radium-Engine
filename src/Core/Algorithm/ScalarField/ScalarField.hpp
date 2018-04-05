#ifndef SCALAR_FIELD_OPERATION
#define SCALAR_FIELD_OPERATION

#include <Core/Containers/VectorArray.hpp> // VectorArray
#include <Core/Mesh/MeshTypes.hpp>         // Triangle
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

// Defining a ScalarField over the mesh vertices
using ScalarField = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

// Defining the Gradient, over the faces of a mesh, of a ScalarField
using Gradient = VectorArray<Vector3>;

// Defining the Divergence, expressend on the vertices, of a Gradient field, expressed on the faces
using Divergence = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

/*
 * Return the Gradient field computed from the ScalarField S defined on the surface of a mesh.
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
RA_CORE_API Gradient gradientOfFieldS( const VectorArray<Vector3>& p,
                                       const VectorArray<Triangle>& T,
                                       const ScalarField& S );

/*
 * Return the Divergence of the Gradient field defined on the surface of a mesh
 *
 * The definition was taken from:
 * "Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow"
 * [Keenan Crane, Clarisse Weischedel, Max Wardetzky ]
 * TOG 2013
 */
RA_CORE_API Divergence divergenceOfFieldX( const VectorArray<Vector3>& p,
                                           const VectorArray<Triangle>& T,
                                           const Gradient& X );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // SCALAR_FIELD_OPERATION
