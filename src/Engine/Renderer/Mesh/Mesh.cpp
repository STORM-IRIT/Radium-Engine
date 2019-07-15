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
Mesh::Mesh( const std::string& name, MeshRenderMode renderMode ) :
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

Mesh::~Mesh() {}

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

size_t Mesh::getNumFaces() const {
    switch ( getRenderMode() )
    {
    case MeshRenderMode::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case MeshRenderMode::RM_TRIANGLE_FAN:
        return ( getTriangleMesh().m_triangles.size() - 1 ) * 3 + 1;
    case MeshRenderMode::RM_TRIANGLES:
        return getTriangleMesh().m_triangles.size();
    default:
        return size_t( 0 );
    }
}

void Mesh::loadGeometry( Core::Geometry::TriangleMesh&& mesh ) {
    m_mesh = std::move( mesh );

    if ( m_mesh.m_triangles.empty() )
    {
        m_numElements = m_mesh.vertices().size();
        setRenderMode( RM_POINTS );
    }
    else
        m_numElements = m_mesh.m_triangles.size() * 3;
    int idx = 0;

    m_dataDirty.resize( m_mesh.vertexAttribs().getNumAttribs() );
    m_vbos.resize( m_mesh.vertexAttribs().getNumAttribs() );

    m_mesh.vertexAttribs().for_each_attrib( [&idx, this]( Ra::Core::Utils::AttribBase* b ) {
        m_handleToBuffer[b->getName()] = idx;
        m_dataDirty[idx]               = true;
        ++idx;
    } );

    m_isDirty = true;
}

void Mesh::loadGeometry( const Core::Vector3Array& vertices, const std::vector<uint>& indices ) {
    // Do not remove this function to force everyone to use TriangleMesh.
    //  ... because we have some line meshes as well...
    Core::Geometry::TriangleMesh mesh;

    auto nIdx = indices.size();

    if ( indices.empty() )
    {
        m_numElements = vertices.size();
        setRenderMode( RM_POINTS );
    }
    else
        m_numElements = nIdx;
    mesh.vertices() = vertices;

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
        mesh.m_triangles.push_back(
            {indices[i], indices[( i + 1 ) % nIdx], indices[( i + 2 ) % nIdx]} );
    }

    m_dataDirty.clear();
    m_vbos.clear();

    ///\todo check line vs triangle here is a bug
    loadGeometry( std::move( mesh ) );
}

void Mesh::addData( const Vec3Data& type, const Core::Vector3Array& data ) {

    if ( !data.empty() )
    {
        auto name = getAttribName( type );
        // add attrib return the corresponding attrib if already present.
        Core::Geometry::TriangleMesh::Vec3AttribHandle handle =
            m_mesh.addAttrib<Core::Vector3>( name );

        //    if ( data.size() != 0 && m_mesh.isValid( handle ) )
        m_mesh.getAttrib( handle ).data() = data;
        auto itr                          = m_handleToBuffer.find( name );

        if ( itr == m_handleToBuffer.end() )
        {
            m_handleToBuffer[name] = m_dataDirty.size();
            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }
        else
            m_dataDirty[m_handleToBuffer[name]] = true;

        m_isDirty = true;
    }
}

void Mesh::addData( const Vec4Data& type, const Core::Vector4Array& data ) {
    if ( !data.empty() )
    {
        auto name = getAttribName( type );
        LOG( logERROR ) << name;

        // add attrib return the corresponding attrib if already present.
        Core::Geometry::TriangleMesh::Vec4AttribHandle handle =
            m_mesh.addAttrib<Core::Vector4>( name );

        //    if ( data.size() != 0 && m_mesh.isValid( handle ) )
        m_mesh.getAttrib( handle ).data() = data;
        auto itr                          = m_handleToBuffer.find( name );
        if ( itr == m_handleToBuffer.end() )
        {
            LOG( logERROR ) << m_dataDirty.size();

            m_handleToBuffer[name] = m_dataDirty.size();
            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }
        else
            m_dataDirty[m_handleToBuffer[name]] = true;

        m_isDirty = true;
    }
}

void Mesh::updateGL() {
    if ( m_isDirty )
    {
        // Check that our dirty bits are consistent.
        ON_ASSERT( bool dirtyTest = false; for ( const auto& d
                                                 : m_dataDirty ) { dirtyTest = dirtyTest || d; } );
        CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency" );
        CORE_ASSERT( !( m_mesh.vertices().empty() ), "No vertex." );

        if ( !m_indices ) { m_indices = globjects::Buffer::create(); }
        if ( m_renderMode == RM_POINTS )
        {
            m_numElements = m_mesh.vertices().size();
            std::vector<int> indices( m_numElements );
            std::iota( indices.begin(), indices.end(), 0 );
            m_indices->setData( indices, GL_STATIC_DRAW );
        }
        else
        {
            m_indices->setData( m_mesh.m_triangles, GL_DYNAMIC_DRAW );
            m_indices->setData( static_cast<gl::GLsizeiptr>( m_mesh.m_triangles.size() *
                                                             sizeof( Core::Vector3ui ) ),
                                m_mesh.m_triangles.data(),
                                GL_STATIC_DRAW );
        }

        auto func = [this]( Ra::Core::Utils::AttribBase* b ) {
            auto idx = m_handleToBuffer[b->getName()];

            LOG( logERROR ) << getName() << "vbo attrib " << b->getName() << "  idx " << idx << "/"
                            << m_vbos.size() << " " << m_dataDirty.size();
            if ( m_dataDirty[idx] )
            {
                if ( !m_vbos[idx] )
                {
                    LOG( logERROR ) << getName() << " create vbo ";
                    m_vbos[idx] = globjects::Buffer::create();
                }
                m_vbos[idx]->setData( b->getBufferSize(), b->dataPtr(), GL_DYNAMIC_DRAW );
                m_dataDirty[idx] = false;
            }
        };
        m_mesh.vertexAttribs().for_each_attrib( func );

        if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
        m_vao->bind();
        m_vao->bindElementBuffer( m_indices.get() );

        for ( auto buffer : m_handleToBuffer )
        {
            // do not remove name from handleToBuffer to keep index ...
            // we could also update handleToBuffer, m_vbos, m_dataDirty
            if ( !m_mesh.hasAttrib( buffer.first ) && m_vbos[buffer.second] )
                m_vbos[buffer.second].reset( nullptr );
        }

        GL_CHECK_ERROR;
        m_isDirty = false;
        m_vao->unbind();
    }
}

void Mesh::updatePickingRenderMode() {
    switch ( getRenderMode() )
    {
    case Mesh::RM_POINTS:
    {
        Displayable::m_pickingRenderMode = PKM_POINTS;
        break;
    }
    case Mesh::RM_LINES: // fall through
        [[fallthrough]];
    case Mesh::RM_LINE_LOOP: // fall through
        [[fallthrough]];
    case Mesh::RM_LINE_STRIP:
    {
        Displayable::m_pickingRenderMode = PKM_LINES;
        break;
    }
    case Mesh::RM_LINES_ADJACENCY: // fall through
    case Mesh::RM_LINE_STRIP_ADJACENCY:
    {
        Displayable::m_pickingRenderMode = PKM_LINE_ADJ;
        break;
    }
    case Mesh::RM_TRIANGLES:
        [[fallthrough]];
    case Mesh::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case Mesh::RM_TRIANGLE_FAN:
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

void Mesh::autoVertexAttribPointer( const ShaderProgram* prog ) {

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
            binding->setBuffer( m_vbos[m_handleToBuffer[name]].get(), 0, attrib->getStride() );
            binding->setFormat( attrib->getElementSize(), GL_FLOAT );
        }
        else
        { m_vao->disable( loc ); }
    }

    m_vao->unbind();
}

void Mesh::setDirty( const Mesh::MeshData& type ) {
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

void Mesh::setDirty( const Vec3Data& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.push_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

void Mesh::setDirty( const Vec4Data& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.push_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

} // namespace Engine
} // namespace Ra
