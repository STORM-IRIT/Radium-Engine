#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>

namespace Ra
{
    namespace Engine
    {
        inline RenderQueue::RenderQueue()
            : Core::AlignedStdVector<BindableMesh>()
        {
        }

        inline void RenderQueue::render() const
        {
            for ( auto& it : *this )
            {
                it.bind();
                it.render();
            }
        }

        inline void RenderQueue::render( const RenderParameters& params ) const
        {
            for ( auto& it : *this )
            {
                it.bind( params );
                it.render();
            }
        }

        inline void RenderQueue::render( ShaderProgram* shader ) const
        {
            for ( auto& it : *this )
            {
                it.bind( shader );
                it.render();
            }
        }

        inline void RenderQueue::render( ShaderProgram* shader, const RenderParameters& params ) const
        {
            for ( auto& it : *this )
            {
                it.bind( shader, params );
                it.render();
            }
        }

    } // namespace Engine
} // namespace Ra
