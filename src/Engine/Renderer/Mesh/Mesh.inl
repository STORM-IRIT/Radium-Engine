#include "Mesh.hpp"

#include <globjects/Buffer.h>

// #include <numeric>

// #include <Core/Utils/Attribs.hpp>
// #include <Core/Utils/Log.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Program.h>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

namespace Ra {
namespace Engine {

void VaoDisplayable::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
    updatePickingRenderMode();
}

template <typename CoreGeometry>
const Ra::Core::Geometry::AbstractGeometry&
DisplayableGeometry<CoreGeometry>::getAbstractGeometry() const {
    return m_mesh;
}

template <typename CoreGeometry>
Ra::Core::Geometry::AbstractGeometry& DisplayableGeometry<CoreGeometry>::getAbstractGeometry() {
    return m_mesh;
}

template <typename CoreGeometry>
const CoreGeometry& DisplayableGeometry<CoreGeometry>::getTriangleMesh() const {
    return m_mesh;
}

template <typename CoreGeometry>
CoreGeometry& DisplayableGeometry<CoreGeometry>::getTriangleMesh() {
    return m_mesh;
}

std::string VaoDisplayable::getAttribName( MeshData type ) {
    if ( type == VERTEX_POSITION ) return {"in_position"};
    if ( type == VERTEX_NORMAL ) return {"in_normal"};
    if ( type == VERTEX_TANGENT ) return {"in_tangent"};
    if ( type == VERTEX_BITANGENT ) return {"in_bitangent"};
    if ( type == VERTEX_TEXCOORD ) return {"in_texcoord"};
    if ( type == VERTEX_COLOR ) return {"in_color"};
    if ( type == VERTEX_WEIGHTS ) return {"in_weight"};
    if ( type == VERTEX_WEIGHT_IDX ) return {"in_weight_idx"};
    return {"invalid mesh data attr name"};
}

template <typename CoreGeometry>
void DisplayableGeometry<CoreGeometry>::addAttribObserver( const std::string& name ) {
    auto attrib = m_mesh.getAttribBase( name );
    // if attrib not nullptr, then it's a add
    if ( attrib )
    {
        auto itr = m_handleToBuffer.find( name );
        if ( itr == m_handleToBuffer.end() )
        {
            m_handleToBuffer[name] = m_dataDirty.size();
            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }
        auto idx = m_handleToBuffer[name];
        attrib->attach( AttribObserver( this, idx ) );
    }
    // else it's a remove, cleanup will be done in updateGL()
    else
    {}
}

template <typename CoreGeometry>
void DisplayableGeometry<CoreGeometry>::autoVertexAttribPointer( const ShaderProgram* prog ) {

    auto glprog           = prog->getProgramObject();
    gl::GLint attribCount = glprog->get( GL_ACTIVE_ATTRIBUTES );

    m_vao->bind();
    for ( GLint idx = 0; idx < attribCount; ++idx )
    {
        const gl::GLsizei bufSize = 256;
        gl::GLchar name[bufSize];
        gl::GLsizei length;
        gl::GLint size;
        gl::GLenum type;
        glprog->getActiveAttrib( idx, bufSize, &length, &size, &type, name );
        auto loc    = glprog->getAttributeLocation( name );
        auto attrib = m_mesh.getAttribBase( name );

        if ( attrib )
        {
            m_vao->enable( loc );
            auto binding = m_vao->binding( idx );
            binding->setAttribute( loc );
            CORE_ASSERT( m_vbos[m_handleToBuffer[name]].get(), "vbo is nullptr" );
            binding->setBuffer( m_vbos[m_handleToBuffer[name]].get(), 0, attrib->getStride() );
            binding->setFormat( attrib->getElementSize(), GL_FLOAT );
        }
        else
        { m_vao->disable( loc ); }
    }

    m_vao->unbind();
}

template <typename T>
void DisplayableGeometry<T>::loadGeometry_common( T&& mesh ) {
    m_mesh  = std::move( mesh );
    int idx = 0;
    m_dataDirty.resize( m_mesh.vertexAttribs().getNumAttribs() );
    m_vbos.resize( m_mesh.vertexAttribs().getNumAttribs() );
    // here capture ref to idx to propagate idx incrementation
    m_mesh.vertexAttribs().for_each_attrib( [&idx, this]( Ra::Core::Utils::AttribBase* b ) {
        m_handleToBuffer[b->getName()] = idx;
        m_dataDirty[idx]               = true;

        b->attach( AttribObserver( this, idx ) );

        ++idx;
    } );
    m_mesh.vertexAttribs().attachMember( this,
                                         &DisplayableGeometry<CoreGeometry>::addAttribObserver );
    m_isDirty = true;
}

template <typename CoreGeometry>
void DisplayableGeometry<CoreGeometry>::updateGL() {
    if ( m_isDirty )
    {
        // Check that our dirty bits are consistent.
        ON_ASSERT( bool dirtyTest = false; for ( const auto& d
                                                 : m_dataDirty ) { dirtyTest = dirtyTest || d; } );
        CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency" );
        CORE_ASSERT( !( m_mesh.vertices().empty() ), "No vertex." );

        updateGL_specific_impl();

        auto func = [this]( Ra::Core::Utils::AttribBase* b ) {
            auto idx = m_handleToBuffer[b->getName()];

            if ( m_dataDirty[idx] )
            {
                if ( !m_vbos[idx] ) { m_vbos[idx] = globjects::Buffer::create(); }
                m_vbos[idx]->setData( b->getBufferSize(), b->dataPtr(), GL_DYNAMIC_DRAW );
                m_dataDirty[idx] = false;
            }
        };
        m_mesh.vertexAttribs().for_each_attrib( func );

        // cleanup removed attrib
        for ( auto buffer : m_handleToBuffer )
        {
            // do not remove name from handleToBuffer to keep index ...
            // we could also update handleToBuffer, m_vbos, m_dataDirty
            if ( !m_mesh.hasAttrib( buffer.first ) && m_vbos[buffer.second] )
            {
                m_vbos[buffer.second].reset( nullptr );
                m_dataDirty[buffer.second] = false;
            }
        }

        GL_CHECK_ERROR;
        m_isDirty = false;
    }
}

template <typename T>
void IndexedGeometry<T>::updateGL_specific_impl() {

    if ( !m_indices )
    {
        m_indices      = globjects::Buffer::create();
        m_indicesDirty = true;
    }
    if ( m_indicesDirty )
    {
        /// this one do not work since m_indices is not a std::vector
        // m_indices->setData( m_mesh.m_indices, GL_DYNAMIC_DRAW );

        m_indices->setData(
            static_cast<gl::GLsizeiptr>( base::m_mesh.m_indices.size() *
                                         sizeof( typename base::CoreGeometry::IndexType ) ),
            base::m_mesh.m_indices.data(),
            GL_STATIC_DRAW );
        m_indicesDirty = false;
    }
    if ( !base::m_vao ) { base::m_vao = globjects::VertexArray::create(); }
    base::m_vao->bind();
    base::m_vao->bindElementBuffer( m_indices.get() );
    base::m_vao->unbind();
}

template <typename T>
void IndexedGeometry<T>::render( const ShaderProgram* prog ) {
    if ( base::m_vao )
    {
        base::autoVertexAttribPointer( prog );
        base::m_vao->bind();
        base::m_vao->drawElements( static_cast<GLenum>( base::m_renderMode ),
                                   GLsizei( m_numElements ),
                                   GL_UNSIGNED_INT,
                                   nullptr );

        base::m_vao->unbind();
    }
}

} // namespace Engine
} // namespace Ra
