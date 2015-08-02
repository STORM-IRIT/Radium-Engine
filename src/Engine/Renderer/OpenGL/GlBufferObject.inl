#include "GlBufferObject.hpp"

namespace Ra { namespace Engine
{

template<class T>
GlBufferObject<T>::GlBufferObject(GLenum type)
    : m_numElements(0)
    , m_targetType(type)
    , m_typeBinding( bufferTypeToBinding(type) )
    , m_drawMode( GL_STREAM_DRAW )
{
    CORE_ASSERT(glGetString(GL_VERSION)!= 0, "GL context unavailable");
    GL_ASSERT( glGenBuffers(1, &m_bufferGlId));
}

template<class T>
GlBufferObject<T>::GlBufferObject(const GlBufferObject<T>& buffer)
    : m_numElements( buffer.m_numElements )
    , m_targetType( buffer.m_targetType )
    , m_typeBinding( buffer.m_typeBinding )
    , m_drawMode( buffer.m_drawMode )
{
    CORE_ASSERT(glGetString(GL_VERSION)!= 0, "GL context unavailable");

    // create vbo_destination to hold part of vbo_src data
    GL_ASSERT( glGenBuffers(1, &m_bufferGlId) );
    bind();
    setData( m_numElements, 0, m_drawMode);

    // rebind buffers to copy from vbo_src to vbo_destination
    //buffer.bind();
    GL_ASSERT( glBindBuffer(GL_COPY_READ_BUFFER, buffer.getId()) );
    GL_ASSERT( glBindBuffer(GL_COPY_WRITE_BUFFER, this->getId()) );
    GL_ASSERT( glCopyBufferSubData(/*buffer.m_targetType*/GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(T) * m_numElements) );
}

template<class T>
GlBufferObject<T>::GlBufferObject(int nb_elt, int type, GLenum draw_mode)
    : m_numElements(nb_elt)
    , m_targetType(type)
    , m_typeBinding( buffer_type_to_binding(type) )
    , m_drawMode( draw_mode )
{
    CORE_ASSERT(glGetString(GL_VERSION)!= 0, "GL context unavailable");

    GL_ASSERT( glGenBuffers(1, &m_bufferGlId) );
    GL_ASSERT( glBindBuffer(m_targetType, m_bufferGlId) );
    GL_ASSERT( glBufferData(m_targetType, m_numElements* sizeof(T), 0, m_drawMode) );
    GL_ASSERT( glBindBuffer(m_targetType, 0) );
}

template<class T>
GlBufferObject<T>::~GlBufferObject()
{
    if( getId() == getCurrentBinding() )
    {
        unbind();
    }
    GL_ASSERT( glDeleteBuffers(1, &m_bufferGlId) );
}

template<class T>
void GlBufferObject<T>::bind() const
{
    GL_ASSERT( glBindBuffer(m_targetType, m_bufferGlId) );
}

template<class T>
void GlBufferObject<T>::unbind() const
{
    GL_ASSERT( glBindBuffer(m_targetType, 0) );
}

template<class T>
void GlBufferObject<T>::set_data(uint numElements,
                               const T* data,
                               GLenum drawMode)
{
    bind();
    m_numElements = numElements;
    m_drawMode = drawMode;
    GL_ASSERT( glBufferData(m_targetType, m_numElements * sizeof(T), (GLvoid*)data, m_drawMode) );
    unbind();
}

template<class T>
void GlBufferObject<T>::setData(const std::vector<T>& data, GLenum drawMode)
{
    setData(data.size(), &data.front(), drawMode);
}

template<class T>
void GlBufferObject<T>::getData(uint numElements
                                T* data, uint offset) const
{
    bind();
    GL_ASSERT( glGetBufferSubData(m_targetType, offset, numElements* sizeof(T), (GLvoid*)data) );
    unbind();
}

template<class T>
T* GlBufferObject<T>::mapTo(GLenum accessMode) const
{
    bind();
    CORE_ASSERT(m_numElements > 0, "No data to map");
    T* data = (T*)glMapBuffer(m_targetType, accessMode);
    GL_CHECK_ERRORS();
    return data;
}

template<class T>
bool GlBufferObject<T>::unmap() const
{
    bind();
    GL_ASSERT(bool state = glUnmapBuffer(m_targetType) ? true : false);
    return state;
}

}}
