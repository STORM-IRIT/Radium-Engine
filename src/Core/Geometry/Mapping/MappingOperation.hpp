#ifndef RADIUMENGINE_MAPPING_OPERATION
#define RADIUMENGINE_MAPPING_OPERATION

#include <Core/Geometry/Mapping/Mapping.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

bool isAllFinite( const Parametrization& param );
bool isAllInside( const Parametrization& param );
bool isAllBoundToElement( const Parametrization& param );

void findParametrization( const TriangleMesh& source, const TriangleMesh& target,
                          Parametrization& param );
void applyParametrization( const TriangleMesh& inMesh, const Parametrization& param,
                           Vector3Array& outPoint, const bool FORCE_DISPLACEMENT_TO_ZERO = false );

void print( const Mapping& map );
void print( const Parametrization& param );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MAPPING_OPERATION
