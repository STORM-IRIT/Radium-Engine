#ifndef RADIUMENGINE_GL_BUFFER_HPP_
#define RADIUMENGINE_GL_BUFFER_HPP_

#include <Core/CoreMacros.hpp>

#include <cassert>
#include <vector>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>


namespace Ra
{
    namespace Engine
    {

        // TODO : unbind and bind should be handle by the user and the buffer should check when bind()   asserting with get_current_binding()

        /**
         * @class GlBuffer
         * @brief Wrapper class for OpenGL buffer objects.
         * @tparam T : the type of the buffer object (float, int, Vec3, float2 etc.)
         * @param type : GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER,
         * GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER
         *
         * TODO : Check if it works with non-contigous data
        */
        template<typename T, GLenum GL_BUFFER_TYPE = GL_ARRAY_BUFFER>
        class GlBuffer
        {
        public:

            /// Creates an empty VBO
            inline GlBuffer();

            /// @param mode  GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
            /// GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
            /// GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
            inline GlBuffer( uint numElements, const T* data = nullptr, GLenum drawMode = GL_STREAM_DRAW );

            inline GlBuffer( const std::vector<T>& data, GLenum drawMode = GL_STREAM_DRAW );

            inline GlBuffer( const GlBuffer<T, GL_BUFFER_TYPE>& buffer );

            inline ~GlBuffer();

            /// Bind the buffer object
            inline void bind() const;

            /// Unbind the buffer object
            inline void unbind() const;


            /// Allocate and upload data of the buffer object
            /// @param nb_elt : number of consecutive elements to be copied in 'data'
            /// @param data : the pointer to the data to be copied or null if no data is
            /// to be copied (allocation only)
            /// @param mode : GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
            /// GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
            /// GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
            inline void setData( uint numElements,
                                 const T* data = nullptr,
                                 GLenum drawMode = GL_STREAM_DRAW );

            /// Allocate and upload data of the buffer object
            /// @param data : vector of datas
            /// @param mode : GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
            /// GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
            /// GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
            inline void setData( const std::vector<T>& data, GLenum drawMode = GL_STREAM_DRAW );

            /// Download data from the buffer object
            void getData( uint numElements,
                          T* data, uint offset = 0 ) const;


            /// Map the buffer object to some pointer on host memory
            /// @param access_mode can be : GL_READ_ONLY, GL_WRITE_ONLY or GL_READ_WRITE.
            /// @return the pointer in host memory mapping to the VBO
            inline T* map( GLenum access_mode ) const;

            /// Unmap the buffer object
            inline bool unmap() const;

            // =========================================================================
            /// @name Getter & Setters
            // =========================================================================

            /// @return buffer object ID OpenGL
            inline GLuint getId() const
            {
                return m_bufferGlId;
            }

            /// @return number of elements inside the BO
            inline uint size() const
            {
                return m_numElements;
            }

        private:
            // =========================================================================
            /// @name Tools
            // =========================================================================

            /// @return the current binding (check same target type as this buffer)
            static GLint getCurrentBinding()
            {
                GLint id;
                GL_ASSERT( glGetIntegerv( bufferTypeToBinding( GL_BUFFER_TYPE ), &id ) );
                return id;
            }

            /// Maps buffer type and binding enums.
            static inline GLenum bufferTypeToBinding( GLenum buffType )
            {
                switch ( buffType )
                {
                    case GL_ARRAY_BUFFER:
                        return GL_ARRAY_BUFFER_BINDING;
                        break;
                    case GL_ELEMENT_ARRAY_BUFFER:
                        return GL_ELEMENT_ARRAY_BUFFER_BINDING;
                        break;
                    case GL_PIXEL_PACK_BUFFER:
                        return GL_PIXEL_PACK_BUFFER_BINDING;
                        break;
                    case GL_PIXEL_UNPACK_BUFFER:
                        return GL_PIXEL_UNPACK_BUFFER_BINDING;
                        break;
                    default:
                        CORE_ASSERT( false, "Unknown buffer type" );
                        return -1;
                        break;
                }
            }

            // =========================================================================
            /// @name Attributes
            // =========================================================================

            uint    m_numElements; ///< number of elements in the buffer
            GLuint m_bufferGlId;  ///< OpenGl id of the buffer object

            /// Latest draw mode used when initializing the buffer
            /// (GL_STREAM_DRAW, GL_STATIC_DRAW etc.)
            GLenum m_drawMode;

        };

    }
}

#include <Engine/Renderer/OpenGL/GlBuffer.inl>

#endif //RADIUMENGINE_GL_BUFFER_OBJECT_HPP_
