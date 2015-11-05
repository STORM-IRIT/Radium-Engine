#ifndef RADIUMENGINE_TEXTURE_HPP
#define RADIUMENGINE_TEXTURE_HPP

#include <Engine/RaEngine.hpp>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API Texture
        {
        public:
            enum TextureType
            {
                TEXTURE_1D = 1 << 0,
                TEXTURE_2D = 1 << 1,
                TEXTURE_3D = 1 << 2,
                TEXTURE_CUBE = 1 << 3
            };

            /**
             * Texture constructor. No OpenGL initialization is done there.
             *
             * @param name Name of the texture
             *
             * @param target OpenGL target of the texture. Given that - for now - ZEngine only handles 3 types of textures,
             * target must be either <b>GL_TEXTURE_1D</b>, <b>GL_TEXTURE_2D</b>, <b>GL_TEXTURE_3D</b> or <b>GL_TEXTURE_CUBE_MAP</b>
             *
             * @param type Internal type of the texture. Can be either <b>TEXTURE_1D</b>, <b>TEXTURE_2D</b>, <b>TEXTURE_3D</b>
             * or <b>TEXTURE_CUBE</b>.
             *
             * @param zoffset The offset of a 2D image within the 3D texture. Not used otherwised.
             *
             * @todo Add anonym textures ?
             * @todo Redundancy between target and type ?
             */
            explicit Texture( std::string name, uint target, TextureType type = TEXTURE_2D, uint zoffset = 0 );

            /**
             * Texture desctructor. Both internal data and OpenGL stuff are deleted.
             */
            ~Texture();

            /**
             * @brief Init the texture 1D from OpenGL point of view.
             *
             * Generate, bind and configure OpenGL texture.<br/>
             * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are generated.<br/><br/>
             *
             * It is highly recommended to take a look at
             * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage1D">glTexImage1D documentation</a>
             * since this method doc will highly refer to it.
             *
             * @param internalFormat The number of color components of the texture, and their size.
             * Refer to the link given above, at the \b internalFormat section
             * for further informations about available internal formats.
             *
             * @param width Width of the 1D texture.
             *
             * @param format The format of the pixel data.
             * Refer to the link given above, at the \b format section
             * for further informations about the available formats.
             *
             * @param type The data type of the pixel data.
             * Refer to the link given above, at the \b type section
             * for further informations about the available types.
             *
             * @param data Data contained in the texture. Can be nullptr. <br/>
             * If \b data is not null, the texture will take the ownership of it.
             */
            void initGL( uint internalFormat, uint width, uint format, uint type, void* data );

            /**
             * @brief Init the texture 2D from OpenGL point of view.
             *
             * Generate, bind and configure OpenGL texture.<br/>
             * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are generated.<br/><br/>
             *
             * It is highly recommended to take a look at
             * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage2D">glTexImage2D documentation</a>
             * since this method doc will highly refer to it.
             *
             * @param internalFormat The number of color components of the texture, and their size.
             * Refer to the link given above, at the \b internalFormat section
             * for further informations about available internal formats.
             *
             * @param width Width of the 2D texture.
             *
             * @param height Height of the 2D texture.
             *
             * @param format The format of the pixel data.
             * Refer to the link given above, at the \b format section
             * for further informations about the available formats.
             *
             * @param type The data type of the pixel data.
             * Refer to the link given above, at the \b type section
             * for further informations about the available types.
             *
             * @param data Data contained in the texture. Can be nullptr. <br/>
             * If \b data is not null, the texture will take the ownership of it.
             */
            void initGL( uint internalFormat, uint width, uint height, uint format, uint type, void* data );

            /**
             * @brief Init the texture 3D from OpenGL point of view.
             *
             * Generate, bind and configure OpenGL texture.<br/>
             * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are generated.<br/><br/>
             *
             * It is highly recommended to take a look at
             * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage3D">glTexImage3D documentation</a>
             * since this method doc will highly refer to it.
             *
             * @param internalFormat The number of color components of the texture, and their size.
             * Refer to the link given above, at the \b internalFormat section
             * for further informations about available internal formats.
             *
             * @param width Width of the 3D texture.
             *
             * @param height Height of the 3D texture.
             *
             * @param depth Depth of the 3D texture.
             *
             * @param format The format of the pixel data.
             * Refer to the link given above, at the \b format section
             * for further informations about the available formats.
             *
             * @param type The data type of the pixel data.
             * Refer to the link given above, at the \b type section
             * for further informations about the available types.
             *
             * @param data Data contained in the texture. Can be nullptr. <br/>
             * If \b data is not null, the texture will take the ownership of it.
             */
            void initGL( uint internalFormat, uint width, uint height, uint depth, uint format, uint type, void* data );

            /**
             * @brief Init the textures needed for the cubemap from OpenGL point of view.
             *
             * Generate, bind and configure OpenGL texture.<br/>
             * Also sets wrapping to GL_REPEAT and min / mag filters to GL_LINEAR, although no mipmaps are generated.<br/><br/>
             *
             * It is highly recommended to take a look at
             * <a href="https://www.opengl.org/wiki/GLAPI/glTexImage2D">glTexImage2D documentation</a>
             * since this method doc will highly refer to it.
             *
             * @param internalFormat The number of color components of the texture, and their size.
             * Refer to the link given above, at the \b internalFormat section
             * for further informations about available internal formats.
             *
             * @param width Width of the six 2D textures.
             *
             * @param height Height of the six 2D textures.
             *
             * @param format The format of the pixel data.
             * Refer to the link given above, at the \b format section
             * for further informations about the available formats.
             *
             * @param type The data type of the pixel data.
             * Refer to the link given above, at the \b type section
             * for further informations about the available types.
             *
             * @param data Data contained in the texture. Can be nullptr. <br/>
             * If \b data is not null, the texture will take the ownership of it.
             */
            void initCubeGL( uint internalFormat, uint width, uint height, uint format, uint type, void** data );

            /**
             * @brief Generate mipmaps for the texture
             *
             * @param minFilter The minifying function used by OpenGL. Must be GL_NEAREST, GL_LINEAR,
             * GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR.<br/>
             *
             * @param magFilter The magnifying function used by OpenGL. Must be GL_NEAREST or GL_LINEAR.
             */
            void genMipmap( uint minFilter, uint magFilter );

            /**
             * @brief Set filter used for textures
             *
             * @param minFilter The minifying function used by OpenGL. Must be GL_NEAREST, GL_LINEAR,
             * GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR.<br/>
             *
             * @param magFilter The magnifying function used by OpenGL. Must be GL_NEAREST or GL_LINEAR.
             */
            void setFilter( uint minFilter, uint magFilter );

            /**
             * @brief Set clamp method for 1D textures.
             *
             * @param warpS Clamp parameter for the s texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             */
            void setClamp( uint warpS );

            /**
             * @brief Set clamp method for both 2D and cube textures.
             *
             * @param warpS Clamp parameter for the s texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             *
             * @param warpT Clamp parameter for the t texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             */
            void setClamp( uint warpS, uint warpT );

            /**
             * @brief Set clamp method for 3D textures.
             *
             * @param warpS Clamp parameter for the s texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             *
             * @param warpT Clamp parameter for the t texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             *
             * @param warpR Clamp parameter for the r texture coordinate.
             * Must be GL_REPEAT, GL_CLAMP_TO_EDGE or GL_MIRRORED_REPEAT.
             */
            void setClamp( uint warpS, uint warpT, uint warpR );

            /**
             * @brief Bind the texture to enable its use in a shader
             * @param unit Index of the texture to be bound.
             */
            void bind( int unit );

            /**
             * @brief Clear OpenGL internal data.
             */
            void deleteGL();

            /**
             * @return OpenGL internal id of the texture.
             */
            uint getId() const;

            /**
             * @return Target of the texture.
             * (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D or GL_TEXTURE_CUBE_MAP)
             */
            uint getTarget() const;

            /**
             * @return Type of the texture.
             * (TEXTURE_1D, TEXTURE_2D, TEXTURE_3D or TEXTURE_CUBE)
             */
            TextureType getType() const;

            /**
             * @return Name of the texture.
             */
            std::string getName() const;

            /**
             * @return Offset of the texture (for a 3D texture).
             */
            uint getZOffset() const;

            Core::Color getTexel( uint u, uint v );

            /**
             * Update the data contained by the texture
             * @param newData The new data, must contain the same number of elements than old data, no check will be performed.
             */
            void updateData( void* newData );

        private:
            Texture( const Texture& ) = delete;
            void operator= ( const Texture& ) = delete;

            void setBPP( int bpp );

        private:
            uint m_textureId;
            std::string m_name;
            uint m_target;
            TextureType m_type;
            uint m_zoffset;

            unsigned char* m_pixels;
            uint m_bytesPerPixel;
            uint m_format;
            uint m_width;
            uint m_height;
            uint m_depth;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TEXTURE_HPP
