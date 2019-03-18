#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <Core/Utils/Index.hpp>
#include <Core/Utils/IndexMap.hpp>

#include <Core/Types.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

namespace Ra {
namespace Engine {
class RenderObject;

/**
 * The RenderObjectManager is responsible for storing the RenderObjects within an Engine.
 */
// FIXME: should go in Engine/Managers
class RA_ENGINE_API RenderObjectManager final {
  public:
    RenderObjectManager();

    ~RenderObjectManager();

    /**
     * Add \p renderObject to the storage and return its storage index.
     */
    Core::Utils::Index addRenderObject( RenderObject* renderObject );

    /**
     * Delete the RenderObject with the valid storage index \p index.
     * \warning Will assert if the index does not match to an existing RenderObject.
     * \see exists().
     */
    void removeRenderObject( const Core::Utils::Index& index );

    /**
     * Return the number of stored RenderObjects.
     */
    size_t getRenderObjectsCount();

    /**
     * Returns the render object corresponding to the given index.
     * \warning Will assert if the index does not match to an existing RenderObject.
     * \see exists().
     */
    std::shared_ptr<RenderObject> getRenderObject( const Core::Utils::Index& index );

    /**
     * Return all RenderObjects into \p objectsOut.
     * \note Previous data in \p objectsOut will be overwritten.
     */
    void getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& objectsOut ) const;

    /**
     * Return all RenderObjects of the given type into \p objectsOut.
     * \note Previous data in \p objectsOut will be overwritten.
     */
    void getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                 const RenderObjectType& type ) const;

    /**
     * Return true if the given Index points to a valid RenderObject.
     */
    bool exists( const Core::Utils::Index& index ) const;

    /**
     * Remove the RenderObject at the given Index.
     * \warning Will assert if the index does not match to an existing RenderObject.
     * \see exists().
     */
    void renderObjectExpired( const Ra::Core::Utils::Index& idx );

    /**
     * Return the total number of faces to be drawn by the RenderObjects.
     */
    size_t getNumFaces() const;

    /**
     * Return the total number of vertices to be drawn by the RednerObjects.
     */
    size_t getNumVertices() const;

    /**
     * Return the AABB of all visible RenderObjects.
     */
    Core::Aabb getSceneAabb() const;

  private:
    /// The list of stored RenderObjects.
    Core::Utils::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;

    /// The per-type list of RenderObject indices.
    std::array<std::set<Core::Utils::Index>, (int)RenderObjectType::Count> m_renderObjectByType;

    /// Guard storage manipulation against thread concurrency.
    mutable std::mutex m_doubleBufferMutex;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
