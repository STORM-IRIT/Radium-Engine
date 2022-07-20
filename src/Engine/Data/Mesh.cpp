#include "Core/Geometry/IndexedGeometry.hpp"
#include <Engine/Data/Mesh.hpp>

#include <numeric>

#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Data/ShaderProgram.hpp>
#include <Engine/OpenGL.hpp>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>

#include <typeinfo>

namespace Ra {
namespace Engine {
namespace Data {
using namespace Ra::Core::Utils;

// Dirty is initializes as false so that we do not create the vao while
// we have no data to send to the gpu.
AttribArrayDisplayable::AttribArrayDisplayable( const std::string& name,
                                                MeshRenderMode renderMode ) :
    Displayable( name ), m_renderMode { renderMode } {
    CORE_ASSERT( m_renderMode == RM_POINTS || m_renderMode == RM_LINES ||
                     m_renderMode == RM_LINE_LOOP || m_renderMode == RM_LINE_STRIP ||
                     m_renderMode == RM_TRIANGLES || m_renderMode == RM_TRIANGLE_STRIP ||
                     m_renderMode == RM_TRIANGLE_FAN || m_renderMode == RM_LINES_ADJACENCY ||
                     m_renderMode == RM_LINE_STRIP_ADJACENCY,
                 "Unsupported render mode" );

    updatePickingRenderMode();
}

size_t Mesh::getNumFaces() const {
    ///\todo fix this once we have explicit triangle fan and strip management.
    switch ( getRenderMode() ) {
    case MeshRenderMode::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case MeshRenderMode::RM_TRIANGLE_FAN:
        return ( getCoreGeometry().getIndices().size() - 1 ) * 3 + 1;
    case MeshRenderMode::RM_TRIANGLES:
        return getCoreGeometry().getIndices().size();
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

    if ( indices.empty() ) {
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
    Core::Geometry::TriangleMesh::IndexContainerType mindices;
    for ( uint i = 0; i < indices.size(); i = i + 3 ) {
        // We store all indices in order. This means that for lines we have
        // (L00, L01, L10), (L11, L20, L21) etc. We fill the missing by wrapping around indices.
        mindices.push_back( { indices[i], indices[( i + 1 ) % nIdx], indices[( i + 2 ) % nIdx] } );
    }

    mesh.setIndices( std::move( mindices ) );
    m_dataDirty.clear();
    m_vbos.clear();

    ///\todo check line vs triangle here is a bug
    loadGeometry( std::move( mesh ) );
}

void AttribArrayDisplayable::updatePickingRenderMode() {
    switch ( getRenderMode() ) {
    case AttribArrayDisplayable::RM_POINTS: {
        Displayable::m_pickingRenderMode = PKM_POINTS;
        break;
    }
    case AttribArrayDisplayable::RM_LINES: // fall through
        [[fallthrough]];
    case AttribArrayDisplayable::RM_LINE_LOOP: // fall through
        [[fallthrough]];
    case AttribArrayDisplayable::RM_LINE_STRIP: {
        Displayable::m_pickingRenderMode = PKM_LINES;
        break;
    }
    case AttribArrayDisplayable::RM_LINES_ADJACENCY: // fall through
        [[fallthrough]];
    case AttribArrayDisplayable::RM_LINE_STRIP_ADJACENCY: {
        Displayable::m_pickingRenderMode = PKM_LINE_ADJ;
        break;
    }
    case AttribArrayDisplayable::RM_TRIANGLES:
        [[fallthrough]];
    case AttribArrayDisplayable::RM_TRIANGLE_STRIP:
        [[fallthrough]];
    case AttribArrayDisplayable::RM_TRIANGLE_FAN: {
        Displayable::m_pickingRenderMode = PKM_TRI;
        break;
    }
    default: {
        Displayable::m_pickingRenderMode = NO_PICKING;
        break;
    }
    }
}

void AttribArrayDisplayable::setDirty( const std::string& name ) {
    auto itr = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() ) {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

void AttribArrayDisplayable::setDirty( unsigned int index ) {
    if ( index < m_dataDirty.size() ) {
        m_dataDirty[index] = true;
        m_isDirty          = true;
    }
}

void AttribArrayDisplayable::setDirty( const Ra::Core::Geometry::MeshAttrib& type ) {
    auto name = Core::Geometry::getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() ) {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}
////////////////  MultiIndexedGeometry  ///////////////////////////////

GeometryDisplayable::GeometryDisplayable( const std::string& name,
                                          typename Core::Geometry::MultiIndexedGeometry&& geom ) :
    base( name ) {
    loadGeometry( std::move( geom ) );
}

GeometryDisplayable::~GeometryDisplayable() {}

void triangulate( Core::Geometry::TriangleIndexLayer& out,
                  const Core::Geometry::PolyIndexLayer& in ) {
    out.collection().clear();
    out.collection().reserve( in.collection().size() );
    for ( const auto& face : in.collection() ) {
        if ( face.size() == 3 ) { out.collection().push_back( face ); }
        else {
            /// simple sew triangulation
            int minus { int( face.size() ) - 1 };
            int plus { 0 };
            while ( plus + 1 < minus ) {
                if ( ( plus - minus ) % 2 ) {
                    out.collection().emplace_back( face[plus], face[plus + 1], face[minus] );
                    ++plus;
                }
                else {
                    out.collection().emplace_back( face[minus], face[plus], face[minus - 1] );
                    --minus;
                }
            }
        }
    }
}

void triangulate( Core::Geometry::TriangleIndexLayer& out,
                  const Core::Geometry::QuadIndexLayer& in ) {
    out.collection().clear();
    out.collection().reserve( 2 * in.getSize() );
    // assume quads are convex
    for ( const auto& face : in.collection() ) {
        out.collection().emplace_back( face[0], face[1], face[2] );
        out.collection().emplace_back( face[0], face[2], face[3] );
    }
}

void GeometryDisplayable::loadGeometry( Core::Geometry::MultiIndexedGeometry&& mesh ) {
    m_geomLayers.clear();
    m_geom = std::move( mesh );
    setupCoreMeshObservers();

    /// \todo check if other layer ? at least triangulate
    if ( m_geom.containsLayer( Core::Geometry::TriangleIndexLayer::staticSemanticName ) ) {
        auto [key, layer] = m_geom.getFirstLayerOccurrence(
            Core::Geometry::TriangleIndexLayer::staticSemanticName );
        addRenderLayer( key, AttribArrayDisplayable::RM_TRIANGLES );
    }
    else if ( m_geom.containsLayer( Core::Geometry::QuadIndexLayer::staticSemanticName ) ) {
        std::cerr << "get quad layer\n";
        auto [key, layer] =
            m_geom.getFirstLayerOccurrence( Core::Geometry::QuadIndexLayer::staticSemanticName );

        std::cerr << "cast quad layer\n";
        std::for_each( layer.semantics().cbegin(),
                       layer.semantics().cend(),
                       []( const std::string& x ) { std::cerr << x << '\n'; } );

        std::cerr << layer.getSize() << " " << layer.getNumberOfComponents() << " "
                  << layer.getBufferSize() << "\n";

        std::cerr << "type info " << typeid( layer ).name() << "\n";
        std::cerr << "type info " << typeid( const Core::Geometry::QuadIndexLayer& ).name() << "\n";

        const auto& quadLayer = dynamic_cast<const Core::Geometry::QuadIndexLayer&>( layer );
        std::cerr << "cast done\n";

        auto triangleLayer = std::make_unique<Core::Geometry::TriangleIndexLayer>();
        std::cerr << "triangulate layer\n";
        triangulate( *triangleLayer, quadLayer );
        std::cerr << "add triangle layer\n";

        LayerKeyType triangleKey = { triangleLayer->semantics(), "triangulation" };
        auto layerAdded = m_geom.addLayer( std::move( triangleLayer ), false, "triangulation" );
        std::cerr << "add triangle layer done\n";

        if ( !layerAdded.first ) { LOG( logERROR ) << "failed to add triangleLayer"; }
        else {
            addRenderLayer( triangleKey, AttribArrayDisplayable::RM_TRIANGLES );
            std::cerr << "triangulate done\n";
        }
        std::cerr << "yo\n";
    }
    else if ( m_geom.containsLayer( Core::Geometry::PolyIndexLayer::staticSemanticName ) ) {
        auto [key, layer] =
            m_geom.getFirstLayerOccurrence( Core::Geometry::PolyIndexLayer::staticSemanticName );

        const auto& polyLayer =
            dynamic_cast<const Core::Geometry::PolyIndexLayer&>( m_geom.getLayer( key ) );

        auto triangleLayer = std::make_unique<Core::Geometry::TriangleIndexLayer>();
        triangulate( *triangleLayer, polyLayer );
        auto layerAdded = m_geom.addLayer( std::move( triangleLayer ), false, "triangulation" );
        if ( !layerAdded.first ) { LOG( logERROR ) << "failed to add triangleLayer"; }
        else {
            LayerKeyType triangleKey = { triangleLayer->semantics(), "triangulation" };
            addRenderLayer( triangleKey, AttribArrayDisplayable::RM_TRIANGLES );
        }
    }
    m_isDirty = true;
}

void GeometryDisplayable::setupCoreMeshObservers() {
    int idx = 0;
    m_dataDirty.resize( m_geom.vertexAttribs().getNumAttribs() );
    m_vbos.resize( m_geom.vertexAttribs().getNumAttribs() );
    // here capture ref to idx to propagate idx incrementation
    m_geom.vertexAttribs().for_each_attrib( [&idx, this]( Ra::Core::Utils::AttribBase* b ) {
        auto name              = b->getName();
        m_handleToBuffer[name] = idx;
        m_dataDirty[idx]       = true;

        // create a identity translation if name is not already translated.
        addToTranslationTable( name );

        b->attach( AttribObserver( this, idx ) );
        ++idx;
    } );

    // add an observer on attrib manipulation.
    m_geom.vertexAttribs().attachMember( this, &GeometryDisplayable::addAttribObserver );
    m_isDirty = true;
}

void GeometryDisplayable::addToTranslationTable( const std::string& name ) {
    auto it = m_translationTableMeshToShader.find( name );
    if ( it == m_translationTableMeshToShader.end() ) {
        m_translationTableMeshToShader[name] = name;
        m_translationTableShaderToMesh[name] = name;
    }
}

void GeometryDisplayable::addAttribObserver( const std::string& name ) {
    // this observer is called each time an attrib is added or removed from m_mesh
    auto attrib = m_geom.getAttribBase( name );
    // if attrib not nullptr, then it's an attrib add, so attach an observer to it

    if ( attrib ) {
        auto itr = m_handleToBuffer.find( name );
        if ( itr == m_handleToBuffer.end() ) {
            m_handleToBuffer[name] = m_dataDirty.size();

            addToTranslationTable( name );

            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }
        auto idx = m_handleToBuffer[name];
        attrib->attach( AttribObserver( this, idx ) );
    }
    // else it's an attrib remove, do nothing, cleanup will be done in updateGL()
    else {}
}

void GeometryDisplayable::setAttribNameCorrespondence( const std::string& meshAttribName,
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

bool GeometryDisplayable::addRenderLayer( LayerKeyType key, base::MeshRenderMode renderMode ) {
    if ( !m_geom.containsLayer( key ) ) return false;
    auto it = m_geomLayers.find( key );
    if ( it != m_geomLayers.end() ) return false;

    m_activeLayerKey = key;
    auto& l          = m_geomLayers.insert( { m_activeLayerKey, LayerEntryType() } ).first->second;

    l.observerId = -1;
    l.renderMode = renderMode;

    return true;
}

bool GeometryDisplayable::removeRenderLayer( LayerKeyType key ) {
    auto it = m_geomLayers.find( key );
    if ( it == m_geomLayers.end() ) return false;

    // the layer might have already been deleted
    if ( m_geom.containsLayer( key ) ) {
        auto& geomLayer = m_geom.getLayerWithLock( key );
        geomLayer.detach( it->second.observerId );
        m_geom.unlockLayer( key );
    }
    it->second.vao.reset();
    m_geomLayers.erase( it );

    return true;
}

void GeometryDisplayable::updateGL() {
    if ( m_isDirty ) {

        const auto& abstractLayer = m_geom.getLayerWithLock( m_activeLayerKey );
        using LayerType           = Ra::Core::Geometry::TriangleIndexLayer;
        const auto& triangleLayer = dynamic_cast<const LayerType&>( abstractLayer );
        // create vao
        auto& l = m_geomLayers[m_activeLayerKey];
        if ( !l.vao ) { l.vao = globjects::VertexArray::create(); }

        auto& vbo = l.indices;
        if ( !vbo.buffer ) {
            vbo.buffer      = globjects::Buffer::create();
            vbo.dirty       = true;
            vbo.numElements = triangleLayer.getSize() * triangleLayer.getNumberOfComponents();
        }

        // upload data to gpu
        if ( vbo.dirty ) {
            vbo.buffer->setData( static_cast<gl::GLsizeiptr>( triangleLayer.getSize() *
                                                              sizeof( LayerType::IndexType ) ),
                                 triangleLayer.collection().data(),
                                 GL_STATIC_DRAW );
            vbo.dirty = false;
        }
        m_geom.unlockLayer( m_activeLayerKey );

        l.vao->bind();
        l.vao->bindElementBuffer( vbo.buffer.get() );
        l.vao->unbind();
        GL_CHECK_ERROR;

        auto func = [this]( Ra::Core::Utils::AttribBase* b ) {
            auto idx = m_handleToBuffer[b->getName()];

            if ( m_dataDirty[idx] ) {
                if ( !m_vbos[idx] ) { m_vbos[idx] = globjects::Buffer::create(); }
                m_vbos[idx]->setData( b->getBufferSize(), b->dataPtr(), GL_DYNAMIC_DRAW );
                m_dataDirty[idx] = false;
            }
        };

        m_geom.vertexAttribs().for_each_attrib( func );

        // cleanup removed attrib
        for ( auto buffer : m_handleToBuffer ) {
            // do not remove name from handleToBuffer to keep index ...
            // we could also update handleToBuffer, m_vbos, m_dataDirty
            if ( !m_geom.hasAttrib( buffer.first ) && m_vbos[buffer.second] ) {
                m_vbos[buffer.second].reset( nullptr );
                m_dataDirty[buffer.second] = false;
            }
        }

        GL_CHECK_ERROR;
        m_isDirty = false;
    }
}

void GeometryDisplayable::render( const ShaderProgram* prog ) {
    if ( m_geomLayers[m_activeLayerKey].vao ) {
        m_geomLayers[m_activeLayerKey].vao->bind();
        autoVertexAttribPointer( prog );
        m_geomLayers[m_activeLayerKey].vao->drawElements(
            static_cast<GLenum>( base::m_renderMode ),
            GLsizei( m_geomLayers[m_activeLayerKey].indices.numElements ),
            GL_UNSIGNED_INT,
            nullptr );
        m_geomLayers[m_activeLayerKey].vao->unbind();
        GL_CHECK_ERROR;
    }
}

void GeometryDisplayable::autoVertexAttribPointer( const ShaderProgram* prog ) {

    auto glprog           = prog->getProgramObject();
    gl::GLint attribCount = glprog->get( GL_ACTIVE_ATTRIBUTES );

    for ( GLint idx = 0; idx < attribCount; ++idx ) {
        const gl::GLsizei bufSize = 256;
        gl::GLchar name[bufSize];
        gl::GLsizei length;
        gl::GLint size;
        gl::GLenum type;
        glprog->getActiveAttrib( idx, bufSize, &length, &size, &type, name );
        auto loc = glprog->getAttributeLocation( name );

        auto attribName = m_translationTableShaderToMesh[name];
        auto attrib     = m_geom.getAttribBase( attribName );

        if ( attrib && attrib->getSize() > 0 ) {
            m_geomLayers[m_activeLayerKey].vao->enable( loc );
            auto binding = m_geomLayers[m_activeLayerKey].vao->binding( idx );

            binding->setAttribute( loc );
            CORE_ASSERT( m_vbos[m_handleToBuffer[attribName]].get(), "vbo is nullptr" );
#ifdef CORE_USE_DOUBLE
            binding->setBuffer( m_vbos[m_handleToBuffer[attribName]].get(),
                                0,
                                attrib->getNumberOfComponents() * sizeof( float ) );
#else

            binding->setBuffer(
                m_vbos[m_handleToBuffer[attribName]].get(), 0, attrib->getStride() );
#endif
            binding->setFormat( attrib->getNumberOfComponents(), GL_SCALAR );
        }
        else { m_geomLayers[m_activeLayerKey].vao->disable( loc ); }
    }
    GL_CHECK_ERROR;
}

Ra::Core::Utils::optional<gl::GLuint> AttribArrayDisplayable::getVaoHandle() {
    if ( m_vao ) return m_vao->id();
    return {};
}
Ra::Core::Utils::optional<gl::GLuint>
AttribArrayDisplayable::getVboHandle( const std::string& name ) {
    auto idx = m_handleToBuffer.find( name );
    if ( idx != m_handleToBuffer.end() && m_vbos[idx->second] ) return m_vbos[idx->second]->id();
    return {};
}

void PointCloud::render( const ShaderProgram* prog ) {
    if ( m_vao ) {
        autoVertexAttribPointer( prog );
        m_vao->bind();
        m_vao->drawArrays(
            static_cast<GLenum>( m_renderMode ), 0, GLsizei( m_mesh.vertices().size() ) );
        m_vao->unbind();
    }
}

void PointCloud::loadGeometry( Core::Geometry::PointCloud&& mesh ) {
    loadGeometry_common( std::move( mesh ) );
}

void PointCloud::updateGL_specific_impl() {
    if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
}

} // namespace Data
} // namespace Engine
} // namespace Ra
