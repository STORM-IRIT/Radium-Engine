#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <numeric>

#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Program.h>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

namespace Ra {
namespace Engine {

using namespace Ra::Core::Utils;

// Dirty is initializes as false so that we do not create the vao while
// we have no data to send to the gpu.
VaoDisplayable::VaoDisplayable( const std::string& name, MeshRenderMode renderMode ) :
    Displayable( name ),
    m_renderMode{renderMode} {
    CORE_ASSERT( m_renderMode == RM_POINTS || m_renderMode == RM_LINES ||
                     m_renderMode == RM_LINE_LOOP || m_renderMode == RM_LINE_STRIP ||
                     m_renderMode == RM_TRIANGLES || m_renderMode == RM_TRIANGLE_STRIP ||
                     m_renderMode == RM_TRIANGLE_FAN || m_renderMode == RM_LINES_ADJACENCY ||
                     m_renderMode == RM_LINE_STRIP_ADJACENCY,
                 "Unsupported render mode" );

    updatePickingRenderMode();
}

void Mesh::render( const ShaderProgram* prog ) {
    if ( m_vao )
    {
        autoVertexAttribPointer( prog );
        m_vao->bind();
        m_vao->drawElements( static_cast<GLenum>( m_renderMode ),
                             GLsizei( m_numElements ),
                             GL_UNSIGNED_INT,
                             nullptr );

        m_vao->unbind();
    }
}

void LineMesh::render( const ShaderProgram* prog ) {
    if ( m_vao )
    {
        autoVertexAttribPointer( prog );
        m_vao->bind();
        m_vao->drawElements( static_cast<GLenum>( m_renderMode ),
                             GLsizei( m_numElements ),
                             GL_UNSIGNED_INT,
                             nullptr );

        m_vao->unbind();
    }
}

size_t Mesh::getNumFaces() const {
    switch ( getRenderMode() )
    {
    case MeshRenderMode::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case MeshRenderMode::RM_TRIANGLE_FAN:
        return ( getTriangleMesh().m_indices.size() - 1 ) * 3 + 1;
    case MeshRenderMode::RM_TRIANGLES:
        return getTriangleMesh().m_indices.size();
    default:
        return size_t( 0 );
    }
}

void Mesh::loadGeometry( const Core::Vector3Array& vertices, const std::vector<uint>& indices ) {
    // Do not remove this function to force everyone to use TriangleMesh.
    //  ... because we have some line meshes as well...
    /// \todo find an alternative
    Core::Geometry::TriangleMesh mesh;

    auto nIdx = indices.size();

    if ( indices.empty() )
    {
        m_numElements = vertices.size();
        setRenderMode( RM_POINTS );
    }
    else
        m_numElements = nIdx;
    mesh.setVertices( vertices );

    // Check that when loading a TriangleMesh we actually have triangles or lines.
    CORE_ASSERT( m_renderMode != GL_TRIANGLES || nIdx % 3 == 0,
                 "There should be 3 indices per triangle " );
    CORE_ASSERT( m_renderMode != GL_LINES || nIdx % 2 == 0, "There should be 2 indices per line" );
    CORE_ASSERT( m_renderMode != GL_LINES_ADJACENCY || nIdx % 4 == 0,
                 "There should be 4 indices per line adjacency" );

    for ( uint i = 0; i < indices.size(); i = i + 3 )
    {
        // We store all indices in order. This means that for lines we have
        // (L00, L01, L10), (L11, L20, L21) etc. We fill the missing by wrapping around indices.
        mesh.m_indices.push_back(
            {indices[i], indices[( i + 1 ) % nIdx], indices[( i + 2 ) % nIdx]} );
    }

    m_dataDirty.clear();
    m_vbos.clear();

    ///\todo check line vs triangle here is a bug
    loadGeometry( std::move( mesh ) );
}

void Mesh::updateGL_specific_impl() {
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
            static_cast<gl::GLsizeiptr>( m_mesh.m_indices.size() * sizeof( Core::Vector3ui ) ),
            m_mesh.m_indices.data(),
            GL_STATIC_DRAW );
        m_indicesDirty = false;
    }
    if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
    m_vao->bind();
    m_vao->bindElementBuffer( m_indices.get() );
    m_vao->unbind();
}

void LineMesh::updateGL_specific_impl() {
    if ( !m_indices )
    {
        m_indices      = globjects::Buffer::create();
        m_indicesDirty = true;
    }
    if ( m_indicesDirty )
    {
        /// this one do not work since m_indices is not a std::vector
        // m_indices->setData( m_mesh.m_indices, GL_DYNAMIC_DRAW );
        m_indices->setData( static_cast<gl::GLsizeiptr>( m_mesh.m_indices.size() * sizeof( int ) ),
                            m_mesh.m_indices.data(),
                            GL_STATIC_DRAW );
        m_indicesDirty = false;
    }
    if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
    m_vao->bind();
    m_vao->bindElementBuffer( m_indices.get() );
    m_vao->unbind();
}

void VaoDisplayable::updatePickingRenderMode() {
    switch ( getRenderMode() )
    {
    case VaoDisplayable::RM_POINTS:
    {
        Displayable::m_pickingRenderMode = PKM_POINTS;
        break;
    }
    case VaoDisplayable::RM_LINES: // fall through
        [[fallthrough]];
    case VaoDisplayable::RM_LINE_LOOP: // fall through
        [[fallthrough]];
    case VaoDisplayable::RM_LINE_STRIP:
    {
        Displayable::m_pickingRenderMode = PKM_LINES;
        break;
    }
    case VaoDisplayable::RM_LINES_ADJACENCY: // fall through
    case VaoDisplayable::RM_LINE_STRIP_ADJACENCY:
    {
        Displayable::m_pickingRenderMode = PKM_LINE_ADJ;
        break;
    }
    case VaoDisplayable::RM_TRIANGLES:
        [[fallthrough]];
    case VaoDisplayable::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case VaoDisplayable::RM_TRIANGLE_FAN:
    {
        Displayable::m_pickingRenderMode = PKM_TRI;
        break;
    }
    default:
    {
        Displayable::m_pickingRenderMode = NO_PICKING;
        break;
    }
    }
}

void VaoDisplayable::setDirty( const std::string& name ) {
    auto itr = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

void VaoDisplayable::setDirty( unsigned int index ) {
    if ( index < m_dataDirty.size() )
    {
        m_dataDirty[index] = true;
        m_isDirty          = true;
    }
}

void VaoDisplayable::setDirty( const VaoDisplayable::MeshData& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

template <>
void DisplayableGeometry<Core::Geometry::TriangleMesh>::loadGeometry(
    Core::Geometry::TriangleMesh&& mesh ) {
    /// \todo  use an helper function instead of 3 here
    m_numElements = mesh.m_indices.size() * 3;
    loadGeometry_common( std::move( mesh ) );
}

template <>
void DisplayableGeometry<Core::Geometry::LineMesh>::loadGeometry(
    Core::Geometry::LineMesh&& mesh ) {
    m_numElements = mesh.m_indices.size() * 2;
    /// \todo remove this ? since it's specified when one create a line mesh.
    setRenderMode( RM_LINES );
    loadGeometry_common( std::move( mesh ) );
}

template <>
void DisplayableGeometry<Core::Geometry::PointCloud>::loadGeometry(
    Core::Geometry::PointCloud&& mesh ) {
    m_numElements = m_mesh.vertices().size();
    setRenderMode( RM_POINTS );
    loadGeometry_common( std::move( mesh ) );
}

void PointCloud::render( const ShaderProgram* prog ) {
    if ( m_vao )
    {
        autoVertexAttribPointer( prog );
        m_vao->bind();
        m_vao->drawArrays( static_cast<GLenum>( m_renderMode ), 0, GLsizei( m_numElements ) );
        m_vao->unbind();
    }
}

void PointCloud::updateGL_specific_impl() {
    m_numElements = m_mesh.vertices().size();
    if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
    base::updateGL();
}

} // namespace Engine
} // namespace Ra
