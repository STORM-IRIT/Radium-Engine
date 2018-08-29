#ifndef RADIUMENGINE_MAPPING_OPERATION
#define RADIUMENGINE_MAPPING_OPERATION

#include <Core/Geometry/Mapping/Mapping.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// Return true if all the mapping values are not nans or infs.
bool isAllFinite( const Parametrization& param );

/// Return true if all the mapping barycentric coordinates are inside the triangle.
bool isAllInside( const Parametrization& param );

/// Return true if all the mapping triangles are valid.
bool isAllBoundToElement( const Parametrization& param );

/// Output the Parametrization from \p source to \p target.
void findParametrization( const TriangleMesh& source, const TriangleMesh& target,
                          Parametrization& param );

/// Apply the given Parametrization to \p inMesh.
/// \note If \p FORCE_DISPLACEMENT_TO_ZERO is set to true, then the vertices will
///       be forced on the mapped triangles, i.e. no displacement.
void applyParametrization( const TriangleMesh& inMesh, const Parametrization& param,
                           Vector3Array& outPoint, const bool FORCE_DISPLACEMENT_TO_ZERO = false );

/// Print the given Mapping to the Info output.
void print( const Mapping& map );

/// Print the given Parametrization to the Info output.
void print( const Parametrization& param );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MAPPING_OPERATION
