#pragma once

#include <Engine/Component/GeometryComponent.hpp>

#include <Core/Containers/MakeShared.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra {
namespace Engine {

template <typename CoreMeshType>
SurfaceMeshComponent<CoreMeshType>::SurfaceMeshComponent(
    const std::string& name,
    Entity* entity,
    const Ra::Core::Asset::GeometryData* data ) :
    GeometryComponent( name, entity ), m_displayMesh( nullptr ) {
    generateMesh( data );
}

template <typename CoreMeshType>
SurfaceMeshComponent<CoreMeshType>::SurfaceMeshComponent( const std::string& name,
                                                          Entity* entity,
                                                          CoreMeshType&& mesh,
                                                          Core::Asset::MaterialData* mat ) :
    GeometryComponent( name, entity ),
    m_displayMesh( new Engine::Mesh( name, std::move( mesh ) ) ) {
    setContentName( name );
    finalizeROFromGeometry( mat, Core::Transform::Identity() );
}

template <typename CoreMeshType>
void SurfaceMeshComponent<CoreMeshType>::generateMesh( const Ra::Core::Asset::GeometryData* data ) {
    m_contentName = data->getName();
    m_displayMesh = Ra::Core::make_shared<RenderMeshType>( m_contentName );

    CoreMeshType mesh;
    typename CoreMeshType::PointAttribHandle::Container vertices;
    typename CoreMeshType::NormalAttribHandle::Container normals;
    typename CoreMeshType::IndexContainerType indices;

    vertices.reserve( data->getVerticesSize() );
    std::copy(
        data->getVertices().begin(), data->getVertices().end(), std::back_inserter( vertices ) );

    if ( data->hasNormals() )
    {
        normals.reserve( data->getVerticesSize() );
        std::copy(
            data->getNormals().begin(), data->getNormals().end(), std::back_inserter( normals ) );
    }

    const auto& faces = data->getFaces();
    indices.reserve( faces.size() );
    std::copy( faces.begin(), faces.end(), std::back_inserter( indices ) );
    mesh.setVertices( std::move( vertices ) );
    mesh.setNormals( std::move( normals ) );

    if ( data->hasTangents() )
    { mesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_TANGENT ), data->getTangents() ); }

    if ( data->hasBiTangents() )
    { mesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_BITANGENT ), data->getBiTangents() ); }

    if ( data->hasTextureCoordinates() )
    { mesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_TEXCOORD ), data->getTexCoords() ); }

    if ( data->hasColors() )
    { mesh.addAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ), data->getColors() ); }

    // To be discussed: Should not weights be part of the geometry ?
    //        mesh->addData( Mesh::VERTEX_WEIGHTS, meshData.weights );

    mesh.setIndices( std::move( indices ) );

    m_displayMesh->loadGeometry( std::move( mesh ) );

    finalizeROFromGeometry( data->hasMaterial() ? &( data->getMaterial() ) : nullptr,
                            data->getFrame() );
}

template <typename CoreMeshType>
void SurfaceMeshComponent<CoreMeshType>::finalizeROFromGeometry(
    const Core::Asset::MaterialData* data,
    Core::Transform transform ) {
    // The technique for rendering this component
    std::shared_ptr<Material> roMaterial;
    // First extract the material from asset or create a default one
    if ( data != nullptr )
    {
        auto converter = EngineMaterialConverters::getMaterialConverter( data->getType() );
        auto mat       = converter.second( data );
        roMaterial.reset( mat );
    }
    else
    {
        auto mat             = new BlinnPhongMaterial( m_contentName + "_DefaultBPMaterial" );
        mat->m_renderAsSplat = m_displayMesh->getNumFaces() == 0;
        mat->m_perVertexColor =
            m_displayMesh->getCoreGeometry().hasAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ) );
        roMaterial.reset( mat );
    }
    // initialize with a default rendertechique that draws nothing
    std::string roName( m_name + "_" + m_contentName + "_RO" );
    auto ro = RenderObject::createRenderObject(
        roName, this, RenderObjectType::Geometry, m_displayMesh, RenderTechnique {} );
    ro->setTransparent( roMaterial->isTransparent() );
    ro->setMaterial( roMaterial );
    setupIO( m_contentName );
    ro->setLocalTransform( transform );
    m_roIndex = addRenderObject( ro );
}

#ifndef CHECK_MESH_NOT_NULL
#    define CHECK_MESH_NOT_NULL                \
        CORE_ASSERT( m_displayMesh != nullptr, \
                     "DisplayMesh should exist while component is alive" );
#    define CHECK_MESH_NOT_NULL_UNDEF
#endif

template <typename CoreMeshType>
const CoreMeshType& SurfaceMeshComponent<CoreMeshType>::getCoreGeometry() const {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh->getCoreGeometry();
}

template <typename CoreMeshType>
typename SurfaceMeshComponent<CoreMeshType>::RenderMeshType*
SurfaceMeshComponent<CoreMeshType>::getDisplayable() {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh.get();
}

template <typename CoreMeshType>
void SurfaceMeshComponent<CoreMeshType>::setupIO( const std::string& id ) {
    CHECK_MESH_NOT_NULL;

    const auto& cm = ComponentMessenger::getInstance();
    auto cbOut     = std::bind( &SurfaceMeshComponent<CoreMeshType>::getMeshOutput, this );
    auto cbRw      = std::bind( &SurfaceMeshComponent<CoreMeshType>::getMeshRw, this );

    cm->registerOutput<CoreMeshType>( getEntity(), this, id, cbOut );
    cm->registerReadWrite<CoreMeshType>( getEntity(), this, id, cbRw );

    base::setupIO( id );
}

template <typename CoreMeshType>
const CoreMeshType* SurfaceMeshComponent<CoreMeshType>::getMeshOutput() const {
    CHECK_MESH_NOT_NULL;
    return &m_displayMesh->getCoreGeometry();
}

template <typename CoreMeshType>
CoreMeshType* SurfaceMeshComponent<CoreMeshType>::getMeshRw() {
    CHECK_MESH_NOT_NULL;
    return &( m_displayMesh->getCoreGeometry() );
}

#ifdef CHECK_MESH_NOT_NULL_UNDEF
#    undef CHECK_MESH_NOT_NULL
#endif

} // namespace Engine
} // namespace Ra
