#ifndef RADIUMENGINE_MESH_CONVERT_HPP
#define RADIUMENGINE_MESH_CONVERT_HPP

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

// Forward declaration
class Index;
struct TriangleMesh;
class Dcel;

struct Twin {
    Twin();
    Twin( const uint i, const uint j );

    bool operator==( const Twin& twin ) const;
    bool operator< ( const Twin& twin ) const;

    uint m_id[2];
};

void convert( const TriangleMesh& mesh, Dcel& dcel );
void convert( const Dcel& dcel, TriangleMesh& mesh );




} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESH_CONVERT_HPP
