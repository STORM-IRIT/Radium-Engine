#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderObject;

        class RA_ENGINE_API RenderObjectManager
        {
        public:
            RenderObjectManager();
            ~RenderObjectManager();

            Core::Index addRenderObject( RenderObject* renderObject );
            void removeRenderObject( const Core::Index& index );
            std::shared_ptr<RenderObject> getRenderObject( const Core::Index& index );

            /**
             * @brief Get all render objects, the vector is assumed to be empty
             * @param Empty vector that will receive render objects
             * @param undirty True if the manager should be marked as clean after calling the method
             */
            void getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& objectsOut, bool undirty = false ) const;

            /**
             * @brief Fill the given vector with renderobjects for a required type.
             * This will do nothing if the given type is not marked dirty, but this will
             * clear the vector and fill it if it's dirty.
             * @param objectsOut Vector of render objects that will be cleared then filled if needed
             * @param type Required type
             * @param undirty True if the given type should be marked as clean
             */
            void getRenderObjectsByTypeIfDirty( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                                const RenderObjectType& type, bool undirty = false ) const;

            void getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                         const RenderObjectType& type, bool undirty = false ) const;

            bool isDirty() const;

            void renderObjectExpired( const Ra::Core::Index& idx );

        private:
            Core::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;

            std::array<std::set<Core::Index>, (int)RenderObjectType::Count> m_renderObjectByType;
            mutable std::array<bool, (int)RenderObjectType::Count> m_typeIsDirty;

            mutable std::mutex m_doubleBufferMutex;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
