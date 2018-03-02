#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <vector>
#include <set>
#include <memory>
#include <mutex>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderObject;

        class RA_ENGINE_API RenderObjectManager final
        {
        public:
            RenderObjectManager();
            ~RenderObjectManager();

            Core::Index addRenderObject( RenderObject* renderObject );
            void removeRenderObject( const Core::Index& index );

            uint getRenderObjectsCount();

            /// Returns the render object corresponding to the given index. Will assert
            /// if the index does not match to an existing render object. See exists()
            std::shared_ptr<RenderObject> getRenderObject( const Core::Index& index );

            /**
             * @brief Get all render objects, the vector is assumed to be empty
             * @param Empty vector that will receive render objects
             */
            void getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& objectsOut) const;

            void getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                         const RenderObjectType& type ) const;

            /// Returns true if the index points to a valid render object.
            bool exists( const Core::Index& index) const;

            void renderObjectExpired( const Ra::Core::Index& idx );

            /// Return the total number of faces drawn
            uint getNumFaces() const;

            /// Return the total number of vertices drawn
            uint getNumVertices() const;

            /// Return the AABB of all visible render objects
            Core::Aabb getSceneAabb() const;

        private:
            Core::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;

            std::array<std::set<Core::Index>, (int)RenderObjectType::Count> m_renderObjectByType;

            mutable std::mutex m_doubleBufferMutex;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
