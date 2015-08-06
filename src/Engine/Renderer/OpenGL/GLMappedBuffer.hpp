#ifndef RADIUMENGINE_GL_MAPPED_BUFFER_HPP_
#define RADIUMENGINE_GL_MAPPED_BUFFER_HPP_

#include <Engine/Renderer/OpenGL/GlBuffer.hpp>

namespace Ra
{
    namespace Engine
    {

        /// This class provides an automated access to an array mapped to an openGL buffer
        /// with access controls.
        template<typename T, GLenum GL_BUFFER_TYPE = GL_ARRAY_BUFFER, GLenum GL_ACCESS_MODE = GL_READ_WRITE>
        class GlMappedBuffer
        {
        public:
            /// Creates a maped buffer on the given opengl buffer object
            GlMappedBuffer ( const GlBuffer<T, GL_BUFFER_TYPE>& buf ) :
                m_mappedArray ( nullptr ),
                m_glBuffer ( buf )
            {
                static_assert ( GL_ACCESS_MODE == GL_READ_ONLY
                                || GL_ACCESS_MODE == GL_WRITE_ONLY
                                || GL_ACCESS_MODE == GL_READ_WRITE, " Unknown access mode." );
            }

            /// Automatic release of the mapped buffer on destrucion.
            ~GlMappedBuffer()
            {
                if ( m_mappedArray )
                {
                    unmap();
                }
            }

            /// Enable the mapping, allowing to read or write from the buffer.
            void map()
            {
                m_mappedArray = m_glBuffer.map_to ( GL_ACCESS_MODE );
            }

            /// Disable the mapping.
            void unmap()
            {
                m_glBuffer.unmap();
                m_mappedArray = nullptr;
            }

            /// Returns the ith element (only if this map has read access).
            const T& get ( uint i ) const
            {
                // Check that we can read the data.
                static_assert ( GL_ACCESS_MODE == GL_READ_ONLY || GL_ACCESS_MODE == GL_READ_WRITE, "Buffer is write only." );

                CORE_ASSERT ( m_mappedArray, "Unmapped buffer." );
                CORE_ASSERT ( i < m_glBuffer.size(), "Out of bounds." );

                return m_mappedArray[i];
            }

            /// Writes the value of the ith element (only if this map has write acces).
            void set ( uint i, const T& value )
            {
                // Check that we can write the data.
                static_assert ( GL_ACCESS_MODE == GL_WRITE_ONLY || GL_ACCESS_MODE == GL_READ_WRITE , "Buffer is read only." );

                CORE_ASSERT ( m_mappedArray, "Unmapped buffer." );
                CORE_ASSERT ( i < m_glBuffer.size(), "Out of bounds." );

                m_mappedArray[i] = value;
            }

        private:
            /// Pointer to the mapped buffer. Set to null if the buffer is unmapped.
            T* m_mappedArray;
            /// The mapped GL buffer object
            const GlBuffer<T, GL_BUFFER_TYPE>& m_glBuffer;
        };

    }
}

#endif // RADIUMENGINE_GL_MAPPED_BUFFER_OBJECT_HPP_
