#ifndef NORMAL_DEFINITION
#define NORMAL_DEFINITION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// \name Mesh normals computation
/// \{

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *       \f[ \frac{ \sum n_j }{ \| \sum n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of the one-ring
 * of vertex \f$ v_i \f$.
 */
RA_CORE_API void uniformNormal( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T,
                                VectorArray<Vector3>& normal );

/**
 * Return the normalized normal of vertex \f$ v_i \f$, expressed as:
 *       \f[ \frac{ \sum n_j }{ \| \sum n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of the one-ring
 * of vertex \f$ v_i \f$.
 */
// FIXME: In the implementation the normal is not normalized,
//        but each face normal is, this can be improved.
RA_CORE_API Vector3 localUniformNormal( const uint i, const VectorArray<Vector3>& p,
                                        const VectorArray<Vector3ui>& T, const Sparse& adj );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *       \f[ \frac{ \sum \theta_j * n_j }{ \| \sum \theta_j * n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of the one-ring
 * of vertex \f$ v_i \f$, and \f$ \theta_j \f$ is its angle at \f$ v_i \f$.
 *
 * The definition was taken from:
 *
 * "Guide to Computational Geometry Processing"
 * [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
 * Chapter 8
 */
RA_CORE_API void angleWeightedNormal( const VectorArray<Vector3>& p,
                                      const VectorArray<Vector3ui>& T,
                                      VectorArray<Vector3>& normal );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *       \f[ \frac{ \sum a_j * n_j }{ \| \sum a_j * n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of the one-ring
 * of vertex \f$ v_i \f$, and \f$ a_j \f$ is its area.
 */
RA_CORE_API void areaWeightedNormal( const VectorArray<Vector3>& p, const VectorArray<Vector3ui>& T,
                                     VectorArray<Vector3>& normal );
/// \}

/// \name One-ring normals computation
/// \{

/**
 * Return the normalized normal of vertex v, expressed as:
 *       \f[ \frac{ \sum n_i }{ \| \sum n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of the one-ring
 * of vertex v.
 */
RA_CORE_API Vector3 uniformNormal( const Vector3& v, const VectorArray<Vector3>& one_ring );

/**
 * Return the normalized normal of vertex v, expressed as:
 *       \f[ \frac{ \sum \theta_i * n_i }{ \| \sum \theta_i * n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of the one-ring
 * of vertex v, and \f$ \theta_i \f$ is its the angle at v.
 *
 * The definition was taken from:
 *
 * "Guide to Computational Geometry Processing"
 * [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
 * Chapter 8
 */
RA_CORE_API Vector3 angleWeightedNormal( const Vector3& v, const VectorArray<Vector3>& one_ring );

/**
 * Return the normalized normal of vertex v, expressed as:
 *       \f[ \frac{ \sum a_i * n_i }{ \| \sum a_i * n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of the one-ring
 * of v, and \f$ a_i \f$ is its area.
 */
RA_CORE_API Vector3 areaWeightedNormal( const Vector3& v, const VectorArray<Vector3>& one_ring );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // NORMAL_DEFINITION
