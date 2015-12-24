#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API RenderObjectManager
        {
        public:
            RenderObjectManager();
            ~RenderObjectManager();

            Core::Index addRenderObject( RenderObject* renderObject );
            void removeRenderObject( const Core::Index& index );
            std::shared_ptr<RenderObject> getRenderObject( const Core::Index& index );

            void getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& objectsOut, bool undirty = false ) const;

            std::shared_ptr<RenderObject> update( uint index, bool cloneMesh = true );
            std::shared_ptr<RenderObject> update( const Core::Index& index, bool cloneMesh = true );
            void doneUpdating( uint index );

            void swapBuffers();

            bool isDirty() const;

        private:
            Core::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;
            std::map<Core::Index, std::shared_ptr<RenderObject>> m_doubleBuffer;

            std::vector<Core::Index> m_doneUpdatingObjects;

            mutable std::mutex m_doubleBufferMutex;
            mutable bool m_isDirty;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
