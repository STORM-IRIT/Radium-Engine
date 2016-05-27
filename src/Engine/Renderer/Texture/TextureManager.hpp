#ifndef RADIUMENGINE_TEXTUREMANAGER_HPP
#define RADIUMENGINE_TEXTUREMANAGER_HPP

#include <Engine/RaEngine.hpp>
#include <map>
#include <string>

#include <Core/Utils/Singleton.hpp>

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
        struct TextureData
        {
            std::string name;
            int width;
            int height;

            GLenum format = GL_RGBA;
            GLenum internalFormat = GL_RGBA8UI;
            GLenum type = GL_UNSIGNED_INT_8_8_8_8;
#ifdef OS_MACOS
            // FIXME (Mathias): GL_CLAMP is deprecated in OpenGL >= 3.0
            GLenum sWrap = GL_CLAMP_TO_EDGE;
            GLenum tWrap = GL_CLAMP_TO_EDGE;
#else
            GLenum sWrap = GL_CLAMP;
            GLenum tWrap = GL_CLAMP;
#endif
            GLenum minMipmap = GL_LINEAR;
            GLenum magMipmap = GL_LINEAR;

            void* data = nullptr;
        };

        class RA_ENGINE_API TextureManager 
        {
            RA_SINGLETON_INTERFACE(TextureManager);
        private:
            typedef std::pair<std::string, Texture*> TexturePair;

        public:
            TextureData& addTexture( const std::string& name, int width, int height, void* data );
            Texture* addTexture( const std::string& filename );
            Texture* getOrLoadTexture( const std::string& filename );
            Texture* getOrLoadTexture( const TextureData& data );

            void deleteTexture( const std::string& filename );
            void deleteTexture( Texture* texture );

            // Call this method to update given texture
            void updateTexture( const std::string& texture, void* data );
            
            // Called by materials
            void updateTextures();

        private:
            TextureManager();
            ~TextureManager();

        private:
            std::map<std::string, Texture*> m_textures;
            std::map<std::string, TextureData> m_pendingTextures;
            std::map<std::string, void*> m_pendingData;
            
            bool m_verbose;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTUREMANAGER_HPP
