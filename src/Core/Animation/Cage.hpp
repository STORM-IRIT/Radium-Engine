#ifndef CAGE_HANDLE_H
#define CAGE_HANDLE_H

#include <Core/Animation/Handle.hpp>
#include <Core/Container/VectorArray.hpp>
#include <Core/Geometry/MeshTypes.hpp> // Triangle
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * The Cage handle class.
 *
 * The Cage handle is a variation of a triangular mesh.
 * Instead of a list of vertices, it owns a list of transforms.
 *
 * \warning This class is probably unused
 */
class RA_CORE_API Cage : public Handle {
public:
    Cage();
    Cage( const uint n );
    Cage( const Cage& cage );

    /// DESTRUCTOR
    ~Cage();

    void clear() override;

    /// VARIABLE
    Container::VectorArray<Geometry::Triangle> m_triangle; // The list of triangles in the cage.
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // CAGE_HANDLE_H
