#ifndef NORMAL_DEFINITION
#define NORMAL_DEFINITION

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Adjacency/Adjacency.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *   \f[ \frac{ \sum n_j }{ \| \sum n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of
 * the one-ring of vertex \f$ v_i \f$.
 */
void RA_CORE_API uniformNormal( const Vector3Array& p, const VectorArray<Triangle>& T,
                                Vector3Array& normal );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *   \f[ \frac{ \sum n_j }{ \| \sum n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of
 * the one-ring of vertex \f$ v_i \f$.
 */
void RA_CORE_API uniformNormal( const Vector3Array& p, const VectorArray<Triangle>& T,
                                const std::vector<Index>& duplicateTable, Vector3Array& normal );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *   \f[ \frac{ \sum n_j }{ \| \sum n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of
 * the one-ring of vertex \f$ v_i \f$.
 */
Vector3 RA_CORE_API localUniformNormal( const uint i, const Vector3Array& p,
                                        const VectorArray<Triangle>& T, const TVAdj& adj );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *   \f[ \frac{ \sum \theta_j * n_j }{ \| \sum \theta_j * n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of
 * the one-ring of vertex \f$ v_i \f$, and \f$ \theta_j \f$ is its angle
 * at \f$ v_i \f$.
 *
 * The definition was taken from:
 * "Guide to Computational Geometry Processing"
 * [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
 * Chapter 8
 */
void RA_CORE_API angleWeightedNormal( const Vector3Array& p, const VectorArray<Triangle>& T,
                                      Vector3Array& normal );

/**
 * Return the normalized normal of each vertex \f$ v_i \f$, expressed as:
 *   \f[ \frac{ \sum a_j * n_j }{ \| \sum a_j * n_j \| } \f]
 *
 * where \f$ n_j \f$ is the normalized normal of the j-th face of
 * the one-ring of vertex \f$ v_i \f$, and \f$ a_j \f$ is its area.
 */
void RA_CORE_API areaWeightedNormal( const Vector3Array& p, const VectorArray<Triangle>& T,
                                     Vector3Array& normal );

/**
 * Return the normalized normal of vertex v, expressed as:
 *   \f[ \frac{ \sum n_i }{ \| \sum n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of
 * the one-ring of vertex v.
 */
Vector3 RA_CORE_API uniformNormal( const Vector3& v, const Vector3Array& one_ring );

/**
 * Return the normalized normal of vertex v, expressed as:
 *   \f[ \frac{ \sum \theta_i * n_i }{ \| \sum \theta_i * n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of
 * the one-ring of vertex v, and \f$ \theta_i \f$ is its the angle at v.
 *
 * The definition was taken from:
 * "Guide to Computational Geometry Processing"
 * [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
 * Chapter 8
 */
Vector3 RA_CORE_API angleWeightedNormal( const Vector3& v, const Vector3Array& one_ring );

/**
 * Return the normalized normal of vertex v, expressed as:
 *   \f[ \frac{ \sum a_i * n_i }{ \| \sum a_i * n_i \| } \f]
 *
 * where \f$ n_i \f$ is the normalized normal of the i-th face of
 * the one-ring of v, and \f$ a_i \f$ is its area.
 */
Vector3 RA_CORE_API areaWeightedNormal( const Vector3& v, const Vector3Array& one_ring );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // NORMAL_DEFINITION
