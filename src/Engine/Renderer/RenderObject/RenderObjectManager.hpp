#ifndef RADIUMENGINE_RENDEROBJECTMANAGER_HPP
#define RADIUMENGINE_RENDEROBJECTMANAGER_HPP

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_API RenderObjectManager
        {
        public:
            RenderObjectManager();
            ~RenderObjectManager();

            Core::Index addRenderObject( RenderObject* renderObject );
            void removeRenderObject( const Core::Index& index );
            std::vector<std::shared_ptr<RenderObject>> getRenderObjects() const;

            std::shared_ptr<RenderObject> update( uint index );
            std::shared_ptr<RenderObject> update( const Core::Index& index );
            void doneUpdating( uint index );

        private:
            Core::IndexMap<std::shared_ptr<RenderObject>> m_renderObjects;
            std::map<Core::Index, std::shared_ptr<RenderObject>> m_doubleBuffer;

            mutable std::mutex m_doubleBufferMutex;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTMANAGER_HPP
