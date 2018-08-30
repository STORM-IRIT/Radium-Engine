#ifndef RADIUMENGINE_MESH_CONVERT_HPP
#define RADIUMENGINE_MESH_CONVERT_HPP

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

// Forward declaration
class Index;
class TriangleMesh;
class Dcel;

/// Stores the fact that two indices are considered twins.
struct RA_CORE_API Twin {
    Twin();
    Twin( const uint i, const uint j );

    /// Return true if both *this and twin consider the same indices.
    bool operator==( const Twin& twin ) const;

    /// Return true if the first index of *this is smaller than \p twin's or
    /// if these are the same and *this's second is smaller than \p twin's second.
    bool operator<( const Twin& twin ) const;

    /// The twin indices, sorted.
    uint m_id[2];
};

/// Convert a TriangleMesh to a Dcel.
RA_CORE_API void convert( const TriangleMesh& mesh, Dcel& dcel );

/// Convert a Dcel to a TriangleMesh.
RA_CORE_API void convert( const Dcel& dcel, TriangleMesh& mesh );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESH_CONVERT_HPP
