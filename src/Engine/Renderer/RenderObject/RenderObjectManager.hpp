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

class RA_ENGINE_API RenderObjectManager final
{
  public:
    RenderObjectManager();
    ~RenderObjectManager();

    Core::Utils::Index addRenderObject( RenderObject* renderObject );
    void removeRenderObject( const Core::Utils::Index& index );

    size_t getRenderObjectsCount();

    /// Returns the render object corresponding to the given index. Will assert
    /// if the index does not match to an existing render object. See exists()
    std::shared_ptr<RenderObject> getRenderObject( const Core::Utils::Index& index );

    /**
     * @brief Get all render objects.
     */
    const Core::Utils::IndexMap<std::shared_ptr<RenderObject>>& getRenderObjects() const;

    /**
     * Get all render objects of the given type, the vector is assumed to be empty whan called
     * @param objectsOut
     * @param type
     */
    void getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                 const RenderObjectType& type ) const;

    /** Returns true if the index points to a valid render object.
     *
     * @param index
     * @return
     */
    bool exists( const Core::Utils::Index& index ) const;

    /**
     * Removed the render object at the given index
     * @param idx
     */
    void renderObjectExpired( const Ra::Core::Utils::Index& idx );

    /** Return the total number of faces drawn
     *
     * @return
     */
    size_t getNumFaces() const;

    /** Return the total number of vertices drawn
     *
     * @return
     */
    size_t getNumVertices() const;

  private:
    Core::Utils::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;

    std::array<std::set<Core::Utils::Index>, (int)RenderObjectType::Count> m_renderObjectByType;

    mutable std::mutex m_doubleBufferMutex;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
