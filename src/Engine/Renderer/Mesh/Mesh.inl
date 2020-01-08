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

////////////////  AttribArrayDisplayable ///////////////////////////////

void AttribArrayDisplayable::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
    updatePickingRenderMode();
}

AttribArrayDisplayable::MeshRenderMode AttribArrayDisplayable::getRenderMode() const {
    return m_renderMode;
}

std::string AttribArrayDisplayable::getAttribName( MeshData type ) {
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
///////////////// VaoIndices  ///////////////////////

void VaoIndices::setIndicesDirty() {
    m_indicesDirty = true;
}

///////////////// IndexedAttribArrayDisplayable ///////////////////////
template <typename I>
template <typename T>
void IndexedAttribArrayDisplayable<I>::addAttrib(
    const std::string& name,
    const typename Ra::Core::Utils::Attrib<T>::Container& data ) {
    auto handle = m_attribManager.addAttrib<T>( name );
    m_attribManager.getAttrib( handle ).setData( data );
    m_handleToBuffer[name] = m_dataDirty.size();
    m_dataDirty.push_back( true );
    m_vbos.emplace_back( nullptr );
    m_isDirty = true;
}

template <typename I>
template <typename T>
void IndexedAttribArrayDisplayable<I>::addAttrib(
    const std::string& name,
    const typename Ra::Core ::Utils::Attrib<T>::Container&& data ) {
    auto handle = m_attribManager.addAttrib<T>( name );
    m_attribManager.getAttrib( handle ).setData( std::move( data ) );
    m_handleToBuffer[name] = m_dataDirty.size();
    m_dataDirty.push_back( true );
    m_vbos.emplace_back( nullptr );
    m_isDirty = true;
}

template <typename I>
void IndexedAttribArrayDisplayable<I>::updateGL() {
    if ( m_isDirty )
    {
        // Check that our dirty bits are consistent.
        ON_ASSERT( bool dirtyTest = false; for ( const auto& d
                                                 : m_dataDirty ) { dirtyTest = dirtyTest || d; } );
        CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency" );

        if ( !m_indices )
        {
            m_indices      = globjects::Buffer::create();
            m_indicesDirty = true;
        }
        if ( m_indicesDirty )
        {
            m_indices->setData(
                static_cast<gl::GLsizeiptr>( m_cpu_indices.size() * sizeof( IndexType ) ),
                m_cpu_indices.data(),
                GL_STATIC_DRAW );
            m_indicesDirty = false;
        }

        m_numElements = m_cpu_indices.size();

        if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
        m_vao->bind();
        m_vao->bindElementBuffer( m_indices.get() );
        m_vao->unbind();

        auto func = [this]( Ra::Core::Utils::AttribBase* b ) {
            auto idx = m_handleToBuffer[b->getName()];

            if ( m_dataDirty[idx] )
            {
                if ( !m_vbos[idx] ) { m_vbos[idx] = globjects::Buffer::create(); }
                m_vbos[idx]->setData( b->getBufferSize(), b->dataPtr(), GL_DYNAMIC_DRAW );
                m_dataDirty[idx] = false;
            }
        };
        m_attribManager.for_each_attrib( func );
        GL_CHECK_ERROR;
        m_isDirty = false;
    }
}

template <typename I>
void IndexedAttribArrayDisplayable<I>::autoVertexAttribPointer( const ShaderProgram* prog ) {

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
        auto loc = glprog->getAttributeLocation( name );

        auto attribName = name; // m_translationTableShaderToMesh[name];
        auto attrib     = m_attribManager.getAttribBase( attribName );

        if ( attrib && attrib->getSize() > 0 )
        {
            m_vao->enable( loc );
            auto binding = m_vao->binding( idx );
            binding->setAttribute( loc );
            CORE_ASSERT( m_vbos[m_handleToBuffer[attribName]].get(), "vbo is nullptr" );
            binding->setBuffer(
                m_vbos[m_handleToBuffer[attribName]].get(), 0, attrib->getStride() );
            binding->setFormat( attrib->getElementSize(), GL_FLOAT );
        }
        else
        { m_vao->disable( loc ); }
    }

    m_vao->unbind();
}

template <typename I>
void IndexedAttribArrayDisplayable<I>::render( const ShaderProgram* prog ) {
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

////////////////  CoreGeometryDisplayable ///////////////////////////////
template <typename CoreGeometry>
CoreGeometryDisplayable<CoreGeometry>::CoreGeometryDisplayable( const std::string& name,
                                                                CoreGeometry&& geom,
                                                                MeshRenderMode renderMode ) :
    AttribArrayDisplayable( name, renderMode ) {
    loadGeometry( std::move( geom ) );
}

template <typename CoreGeometry>
const Ra::Core::Geometry::AbstractGeometry&
CoreGeometryDisplayable<CoreGeometry>::getAbstractGeometry() const {
    return m_mesh;
}

template <typename CoreGeometry>
Ra::Core::Geometry::AbstractGeometry& CoreGeometryDisplayable<CoreGeometry>::getAbstractGeometry() {
    return m_mesh;
}

template <typename CoreGeometry>
const CoreGeometry& CoreGeometryDisplayable<CoreGeometry>::getCoreGeometry() const {
    return m_mesh;
}

template <typename CoreGeometry>
CoreGeometry& CoreGeometryDisplayable<CoreGeometry>::getCoreGeometry() {
    return m_mesh;
}

template <typename CoreGeometry>
void CoreGeometryDisplayable<CoreGeometry>::addAttribObserver( const std::string& name ) {
    // this observer is called each time an attrib is added or removed from m_mesh
    auto attrib = m_mesh.getAttribBase( name );
    // if attrib not nullptr, then it's an attrib add, so attach an observer to it
    if ( attrib )
    {
        auto itr = m_handleToBuffer.find( name );
        if ( itr == m_handleToBuffer.end() )
        {
            m_handleToBuffer[name] = m_dataDirty.size();

            auto it = m_translationTableMeshToShader.find( name );
            if ( it == m_translationTableMeshToShader.end() )
            {
                m_translationTableMeshToShader[name] = name;
                m_translationTableShaderToMesh[name] = name;
            }

            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }
        auto idx = m_handleToBuffer[name];
        attrib->attach( AttribObserver( this, idx ) );
    }
    // else it's an attrib remove, do nothing, cleanup will be done in updateGL()
    else
    {}
}

template <typename CoreGeometry>
void CoreGeometryDisplayable<CoreGeometry>::autoVertexAttribPointer( const ShaderProgram* prog ) {

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
        auto loc = glprog->getAttributeLocation( name );

        auto attribName = m_translationTableShaderToMesh[name];
        auto attrib     = m_mesh.getAttribBase( attribName );

        if ( attrib && attrib->getSize() > 0 )
        {
            m_vao->enable( loc );
            auto binding = m_vao->binding( idx );
            binding->setAttribute( loc );
            CORE_ASSERT( m_vbos[m_handleToBuffer[attribName]].get(), "vbo is nullptr" );
            binding->setBuffer(
                m_vbos[m_handleToBuffer[attribName]].get(), 0, attrib->getStride() );
            binding->setFormat( attrib->getElementSize(), GL_FLOAT );
        }
        else
        { m_vao->disable( loc ); }
    }

    m_vao->unbind();
}

template <typename T>
void CoreGeometryDisplayable<T>::loadGeometry_common( T&& mesh ) {
    m_mesh  = std::move( mesh );
    int idx = 0;
    m_dataDirty.resize( m_mesh.vertexAttribs().getNumAttribs() );
    m_vbos.resize( m_mesh.vertexAttribs().getNumAttribs() );
    // here capture ref to idx to propagate idx incrementation
    m_mesh.vertexAttribs().for_each_attrib( [&idx, this]( Ra::Core::Utils::AttribBase* b ) {
        auto name              = b->getName();
        m_handleToBuffer[name] = idx;
        m_dataDirty[idx]       = true;

        // create a identity translation if name is not already translated.
        auto it = m_translationTableMeshToShader.find( name );
        if ( it == m_translationTableMeshToShader.end() )
        {
            m_translationTableMeshToShader[name] = name;
            m_translationTableShaderToMesh[name] = name;
        }

        b->attach( AttribObserver( this, idx ) );
        ++idx;
    } );
    m_mesh.vertexAttribs().attachMember(
        this, &CoreGeometryDisplayable<CoreGeometry>::addAttribObserver );
    m_isDirty = true;
}

/// Helper function that calls Ra::Core::CoreGeometry::addAttrib()
template <typename CoreGeometry>
template <typename A>
Ra::Core::Utils::AttribHandle<A>
CoreGeometryDisplayable<CoreGeometry>::addAttrib( const std::string& name,
                                                  const typename Core::VectorArray<A>& data ) {
    return m_mesh.addAttrib( name, data );
}

template <typename CoreGeometry>
size_t CoreGeometryDisplayable<CoreGeometry>::getNumVertices() const {
    return m_mesh.vertices().size();
}

template <typename CoreGeometry>
void CoreGeometryDisplayable<CoreGeometry>::loadGeometry( CoreGeometry&& /*mesh*/ ) {
    CORE_ASSERT( false, "must be specialized" );
}

template <typename CoreGeometry>
void CoreGeometryDisplayable<CoreGeometry>::updateGL() {
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

template <typename CoreGeometry>
void CoreGeometryDisplayable<CoreGeometry>::setAttribNameCorrespondance(
    const std::string& meshAttribName,
    const std::string& shaderAttribName ) {

    // clean previously set translation

    auto it1 = std::find_if( m_translationTableShaderToMesh.begin(),
                             m_translationTableShaderToMesh.end(),
                             [&meshAttribName]( const TranslationTable::value_type& p ) {
                                 return p.second == meshAttribName;
                             } );

    if ( it1 != m_translationTableShaderToMesh.end() ) m_translationTableShaderToMesh.erase( it1 );

    auto it2 = std::find_if( m_translationTableMeshToShader.begin(),
                             m_translationTableMeshToShader.end(),
                             [&shaderAttribName]( const TranslationTable::value_type& p ) {
                                 return p.second == shaderAttribName;
                             } );

    if ( it2 != m_translationTableMeshToShader.end() ) m_translationTableMeshToShader.erase( it2 );

    m_translationTableShaderToMesh[shaderAttribName] = meshAttribName;
    m_translationTableMeshToShader[meshAttribName]   = shaderAttribName;
}

////////////////  IndexedGeometry  ///////////////////////////////

template <typename T>
IndexedGeometry<T>::IndexedGeometry( const std::string& name,
                                     typename base::CoreGeometry&& geom,
                                     typename base::MeshRenderMode renderMode ) :
    base( name, renderMode ) {
    loadGeometry( std::move( geom ) );
}

template <typename T>
void IndexedGeometry<T>::loadGeometry( T&& mesh ) {
    setIndicesDirty();
    m_numElements = mesh.m_indices.size() * base::CoreGeometry::IndexType::RowsAtCompileTime;
    base::loadGeometry_common( std::move( mesh ) );
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
        GL_CHECK_ERROR;
        base::m_vao->bind();
        GL_CHECK_ERROR;
        base::m_vao->drawElements( static_cast<GLenum>( base::m_renderMode ),
                                   GLsizei( m_numElements ),
                                   GL_UNSIGNED_INT,
                                   nullptr );
        GL_CHECK_ERROR;
        base::m_vao->unbind();
        GL_CHECK_ERROR;
    }
}

/////////  LineMesh ///////////
LineMesh::LineMesh( const std::string& name,
                    typename base::CoreGeometry&& geom,
                    typename base::MeshRenderMode renderMode ) :
    base( name, std::move( geom ), renderMode ) {}

} // namespace Engine
} // namespace Ra
