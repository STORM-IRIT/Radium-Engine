#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

namespace Ra {
namespace Engine {
class RenderObject;

/// The renderObjectManager is responsible for storing the RenderObjects within an Engine.
class RA_ENGINE_API RenderObjectManager final {
  public:
    RenderObjectManager();

    ~RenderObjectManager();

    /// Add \p renderObject to the storage and return its storage index.
    Core::Index addRenderObject( RenderObject* renderObject );

    /// Delete the RenderObject with the valid storage index \p index.
    /// \note Will assert if the index does not match to an existing render object.
    /// \see exists().
    void removeRenderObject( const Core::Index& index );

    /// Return the number of stored RenderObjects.
    uint getRenderObjectsCount();

    /// Returns the render object corresponding to the given index
    /// \note Will assert if the index does not match to an existing render object.
    /// \see exists().
    std::shared_ptr<RenderObject> getRenderObject( const Core::Index& index );

    /// Output all the stored RenderObjects.
    void getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& objectsOut ) const;

    /// Output all the stored RenderObjects of type \p type..
    void getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                 const RenderObjectType& type ) const;

    /// Returns true if the index points to a valid render object.
    bool exists( const Core::Index& index ) const;

    /// Remove the RenderObject with the valid storage index \p index.
    /// \note Does not fire the RenderObject deletion signal.
    void renderObjectExpired( const Ra::Core::Index& idx );

    /// Return the total number of faces drawn.
    uint getNumFaces() const;

    /// Return the total number of vertices drawn.
    uint getNumVertices() const;

    /// Return the AABB of all visible render objects.
    Core::Aabb getSceneAabb() const;

  private:
    /// The list of stored RenderObjects.
    Core::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;

    /// The per-type list of RenderObject indices.
    std::array<std::set<Core::Index>, (int)RenderObjectType::Count> m_renderObjectByType;

    /// Guard storage manipulation against thread concurrency.
    mutable std::mutex m_doubleBufferMutex;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
