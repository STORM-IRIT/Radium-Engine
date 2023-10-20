#pragma once
#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

namespace Ra::Core::Asset {
class GeometryData;
}

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Functor that computes normals of a triangle meshmesh
 */
class NormalCalculator
{
  public:
    /** Compute the normals for the given mesh
     * @note : assume that both vertices and faces are set on the geometry data
     *
     * @param gdp The geometry data on which the normal must be computed
     * @param basic true if vertices' normals must be face normal average, false if normals must be
     * area-weighted average
     */
    void operator()( Ra::Core::Asset::GeometryData* gdp, bool basic = true );

  private:
    static Ra::Core::Vector3 getTriangleNormal( const Ra::Core::Vector3ui& t,
                                                bool basic,
                                                const Ra::Core::Vector3Array& vertices );
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
