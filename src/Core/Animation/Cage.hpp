#ifndef CAGE_HANDLE_H
#define CAGE_HANDLE_H

#include <Core/Animation/HandleArray.hpp>
#include <Core/Containers/VectorArray.hpp>
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
class RA_CORE_API Cage : public HandleArray
{
  public:
    Cage();
    explicit Cage( const uint n );
    Cage( const Cage& cage );

    /// DESTRUCTOR
    ~Cage();

    void clear() override;

    /**
     * The list of triangles in the cage.
     */
    VectorArray<Ra::Core::Vector3ui> m_triangle;
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // CAGE_HANDLE_H
