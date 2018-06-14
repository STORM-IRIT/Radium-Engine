#ifndef RADIUMENGINE_MESH_CONVERT_HPP
#define RADIUMENGINE_MESH_CONVERT_HPP

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

// Forward declaration
class Index;
class TriangleMesh;
class Dcel;

struct RA_CORE_API Twin {
    Twin();
    Twin( const uint i, const uint j );

    bool operator==( const Twin& twin ) const;
    bool operator<( const Twin& twin ) const;

    uint m_id[2];
};

RA_CORE_API void convert( const TriangleMesh& mesh, Dcel& dcel );
RA_CORE_API void convert( const Dcel& dcel, TriangleMesh& mesh );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESH_CONVERT_HPP
