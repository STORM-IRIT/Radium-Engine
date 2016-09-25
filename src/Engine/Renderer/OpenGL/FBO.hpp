#ifndef RADIUMENGINE_FBO_HPP
#define RADIUMENGINE_FBO_HPP

#include <Engine/RaEngine.hpp>

#include <map>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;
    }
}

namespace Ra
{
    namespace Engine
    {

        class Texture;

        class RA_ENGINE_API FBO
        {
        public:
            enum Component
            {
                Component_Color = 1 << 0,
                Component_Depth = 1 << 1,
                Component_Stencil = 1 << 2,
                Component_All = Component_Color | Component_Depth | Component_Stencil
            };

            FBO(int components, uint width, uint height );
            ~FBO();

            void bind();

            // Binds and sets viewport
            void useAsTarget();
            void useAsTarget( uint width, uint height );
            void unbind( bool complete = false );

            void attachTexture(uint attachment, Texture* texture);
            void detachTexture(uint attachment);

            void check() const;

            void setSize( uint width, uint height );
            void clear( Component components );

            uint getID() const
            {
                return m_fboID;
            }

        private:
            FBO( const FBO& ) = delete;
            void operator= ( const FBO& ) = delete;

        private:
            uint m_fboID;
            int m_components;
            uint m_width, m_height;

            std::map<uint, Texture*> m_textures;

            bool m_isBound;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FBO_HPP
