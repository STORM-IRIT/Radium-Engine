#include <Engine/Component/GeometryComponent.hpp>

#include <iostream>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/Normal.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Utils/Color.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Engine/Renderer/Displayable/VolumeObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>
#include <Engine/Renderer/Material/VolumetricMaterial.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

using TriangleArray = Ra::Core::VectorArray<Ra::Core::Vector3ui>;

#define CHECK_MESH_NOT_NULL \
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );

#include <Core/Utils/Log.hpp>
using namespace Ra::Core::Utils;

namespace Ra {
namespace Engine {

void GeometryComponent::setupIO( const std::string& id ) {
    const auto& cm = ComponentMessenger::getInstance();
    auto roOut     = std::bind( &GeometryComponent::roIndexRead, this );
    cm->registerOutput<Index>( getEntity(), this, id, roOut );
}

const Index* GeometryComponent::roIndexRead() const {
    return &m_roIndex;
}

TriangleMeshComponent::TriangleMeshComponent( const std::string& name,
                                              Entity* entity,
                                              const Ra::Core::Asset::GeometryData* data ) :
    GeometryComponent( name, entity ), m_displayMesh( nullptr ) {
    generateTriangleMesh( data );
}

TriangleMeshComponent::TriangleMeshComponent( const std::string& name,
                                              Entity* entity,
                                              Core::Geometry::TriangleMesh&& mesh,
                                              Core::Asset::MaterialData* mat ) :
    GeometryComponent( name, entity ), m_displayMesh( new Engine::Mesh( name ) ) {
    setContentName( name );
    m_displayMesh->loadGeometry( std::move( mesh ) );
    finalizeROFromGeometry( mat );
}

//////////// STORE Mesh/TriangleMesh here instead of an index, so don't need to request the ROMgr
/// and no problem with the Displayable -> doesn't affect the API

TriangleMeshComponent::~TriangleMeshComponent() = default;

void TriangleMeshComponent::generateTriangleMesh( const Ra::Core::Asset::GeometryData* data ) {
    m_contentName = data->getName();

    std::string name( m_name );
    name.append( "_" + m_contentName );

    std::string meshName = name;
    meshName.append( "_Mesh" );

    m_displayMesh = Ra::Core::make_shared<Mesh>( meshName );

    Ra::Core::Geometry::TriangleMesh mesh;
    Ra::Core::Geometry::TriangleMesh::PointAttribHandle::Container vertices;
    Ra::Core::Geometry::TriangleMesh::NormalAttribHandle::Container normals;

    const auto T = data->getFrame();
    const Ra::Core::Transform N( ( T.matrix() ).inverse().transpose() );

    vertices.resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );

#pragma omp parallel for
    for ( int i = 0; i < int( data->getVerticesSize() ); ++i )
    {
        vertices[i] = T * data->getVertices()[i];
    }

    if ( data->hasNormals() )
    {
        normals.resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );
#pragma omp parallel for
        for ( int i = 0; i < int( data->getVerticesSize() ); ++i )
        {
            normals[i] = ( N * data->getNormals()[i] ).normalized();
        }
    }

    const auto& faces = data->getFaces();
    mesh.m_indices.resize( faces.size(), Ra::Core::Vector3ui::Zero() );
#pragma omp parallel for
    for ( int i = 0; i < int( faces.size() ); ++i )
    {
        mesh.m_indices[i] = faces[i].head<3>();
    }

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

    m_displayMesh->loadGeometry( std::move( mesh ) );

    finalizeROFromGeometry( data->hasMaterial() ? &( data->getMaterial() ) : nullptr );
}

void TriangleMeshComponent::finalizeROFromGeometry( const Core::Asset::MaterialData* data ) {
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
        roName, this, RenderObjectType::Geometry, m_displayMesh, RenderTechnique{} );
    ro->setTransparent( roMaterial->isTransparent() );
    ro->setMaterial( roMaterial );
    setupIO( m_contentName );
    m_roIndex = addRenderObject( ro );
}

const Ra::Core::Geometry::TriangleMesh& TriangleMeshComponent::getCoreGeometry() const {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh->getCoreGeometry();
}

Mesh* TriangleMeshComponent::getDisplayable() {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh.get();
}

void TriangleMeshComponent::setupIO( const std::string& id ) {
    CHECK_MESH_NOT_NULL;

    const auto& cm = ComponentMessenger::getInstance();
    auto cbOut     = std::bind( &TriangleMeshComponent::getMeshOutput, this );
    auto cbRw      = std::bind( &TriangleMeshComponent::getMeshRw, this );

    cm->registerOutput<Ra::Core::Geometry::TriangleMesh>( getEntity(), this, id, cbOut );
    cm->registerReadWrite<Ra::Core::Geometry::TriangleMesh>( getEntity(), this, id, cbRw );

    base::setupIO( id );
}

const Ra::Core::Geometry::TriangleMesh* TriangleMeshComponent::getMeshOutput() const {
    CHECK_MESH_NOT_NULL;
    return &m_displayMesh->getCoreGeometry();
}

Ra::Core::Geometry::TriangleMesh* TriangleMeshComponent::getMeshRw() {
    CHECK_MESH_NOT_NULL;
    return &( m_displayMesh->getCoreGeometry() );
}

/*-----------------------------------------------------------------------------------------------*/
/*---------------------------------  PointCloud Component----------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/

PointCloudComponent::PointCloudComponent( const std::string& name,
                                          Entity* entity,
                                          const Ra::Core::Asset::GeometryData* data ) :
    GeometryComponent( name, entity ), m_displayMesh( nullptr ) {
    generatePointCloud( data );
}

PointCloudComponent::PointCloudComponent( const std::string& name,
                                          Entity* entity,
                                          Core::Geometry::PointCloud&& mesh,
                                          Core::Asset::MaterialData* mat ) :
    GeometryComponent( name, entity ), m_displayMesh( new Engine::PointCloud( name ) ) {
    m_displayMesh->loadGeometry( std::move( mesh ) );
    finalizeROFromGeometry( mat );
}

//////////// STORE Mesh/PointCloud here instead of an index, so don't need to request the ROMgr
/// and no problem with the Displayable -> doesn't affect the API

PointCloudComponent::~PointCloudComponent() = default;

void PointCloudComponent::initialize() {}

void PointCloudComponent::generatePointCloud( const Ra::Core::Asset::GeometryData* data ) {
    m_contentName = data->getName();

    std::string name( m_name );
    name.append( "_" + m_contentName );

    std::string meshName = name;
    meshName.append( "_PointCloud" );

    m_displayMesh = Ra::Core::make_shared<PointCloud>( meshName );
    m_displayMesh->setRenderMode( AttribArrayDisplayable::RM_POINTS );

    Ra::Core::Geometry::PointCloud mesh;
    Ra::Core::Geometry::PointCloud::PointAttribHandle::Container vertices;
    Ra::Core::Geometry::PointCloud::NormalAttribHandle::Container normals;

    const auto T = data->getFrame();
    const Ra::Core::Transform N( ( T.matrix() ).inverse().transpose() );

    vertices.resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );

#pragma omp parallel for
    for ( int i = 0; i < int( data->getVerticesSize() ); ++i )
    {
        vertices[i] = T * data->getVertices()[i];
    }

    if ( data->hasNormals() )
    {
        normals.resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );
#pragma omp parallel for
        for ( int i = 0; i < int( data->getVerticesSize() ); ++i )
        {
            normals[i] = ( N * data->getNormals()[i] ).normalized();
        }
    }

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

    m_displayMesh->loadGeometry( std::move( mesh ) );

    finalizeROFromGeometry( data->hasMaterial() ? &( data->getMaterial() ) : nullptr );
}

void PointCloudComponent::finalizeROFromGeometry( const Core::Asset::MaterialData* data ) {
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
        roName, this, RenderObjectType::Geometry, m_displayMesh, RenderTechnique{} );
    ro->setTransparent( roMaterial->isTransparent() );
    ro->setMaterial( roMaterial );
    setupIO( m_contentName );
    m_roIndex = addRenderObject( ro );
}

const Ra::Core::Geometry::PointCloud& PointCloudComponent::getCoreGeometry() const {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh->getCoreGeometry();
}

PointCloud* PointCloudComponent::getGeometry() {
    CHECK_MESH_NOT_NULL;
    return m_displayMesh.get();
}

void PointCloudComponent::setupIO( const std::string& id ) {
    CHECK_MESH_NOT_NULL;
    auto cbOut = std::bind( &PointCloudComponent::getMeshOutput, this );
    auto cbRw  = std::bind( &PointCloudComponent::getPointCloudRw, this );

    const auto& cm = ComponentMessenger::getInstance();

    cm->registerOutput<Ra::Core::Geometry::PointCloud>( getEntity(), this, id, cbOut );
    cm->registerReadWrite<Ra::Core::Geometry::PointCloud>( getEntity(), this, id, cbRw );
    base::setupIO( id );
}

const Ra::Core::Geometry::PointCloud* PointCloudComponent::getMeshOutput() const {
    CHECK_MESH_NOT_NULL;
    return &m_displayMesh->getCoreGeometry();
}

Ra::Core::Geometry::PointCloud* PointCloudComponent::getPointCloudRw() {
    CHECK_MESH_NOT_NULL;
    return &( m_displayMesh->getCoreGeometry() );
}

/*-----------------------------------------------------------------------------------------------*/
/*---------------------------------  Volume Component  ------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
VolumeComponent::VolumeComponent( const std::string& name,
                                  Entity* entity,
                                  const Ra::Core::Asset::VolumeData* data ) :
    Component( name, entity ), m_displayVolume{nullptr} {
    generateVolumeRender( data );
}

VolumeComponent::~VolumeComponent() = default;

void VolumeComponent::initialize() {}

void VolumeComponent::generateVolumeRender( const Ra::Core::Asset::VolumeData* data ) {
    std::string name( m_name );
    name.append( "_" + data->getName() );

    std::string roName = name;

    roName.append( "_RO" );
    std::string meshName = name;
    meshName.append( "_Mesh" );

    std::string matName = name;
    matName.append( "_Mat" );

    m_contentName = name + "_DAT_" + data->getName();

    m_displayVolume = Ra::Core::make_shared<VolumeObject>( meshName );
    m_displayVolume->loadGeometry( data->volume, data->boundingBox );

    auto roMaterial = Ra::Core::make_shared<VolumetricMaterial>( data->getName() + "_VolMat" );
    roMaterial->setTexture( const_cast<Texture*>( &( m_displayVolume->getDataTexture() ) ) );
    roMaterial->m_sigma_a       = data->sigma_a;
    roMaterial->m_sigma_s       = data->sigma_s;
    roMaterial->m_modelToMedium = data->densityToModel.inverse();

    auto ro = RenderObject::createRenderObject(
        roName, this, RenderObjectType::Geometry, m_displayVolume, RenderTechnique{} );
    ro->setTransparent( roMaterial->isTransparent() );
    ro->setMaterial( roMaterial );
    ro->setLocalTransform( data->modelToWorld );

    setupIO( m_contentName );
    m_volumeIndex = addRenderObject( ro );
}

VolumeObject* VolumeComponent::getDisplayVolume() {
    CORE_ASSERT( m_displayVolume != nullptr,
                 "DisplayVolume should exist while component is alive" );
    return m_displayVolume.get();
}

Index VolumeComponent::getRenderObjectIndex() const {
    CORE_ASSERT( m_displayVolume != nullptr,
                 "DisplayVolume should exist while component is alive" );
    return m_volumeIndex;
}

void VolumeComponent::setContentName( const std::string& name ) {
    this->m_contentName = name;
}

void VolumeComponent::setupIO( const std::string& id ) {
    CORE_ASSERT( m_displayVolume != nullptr,
                 "DisplayVolume should exist while component is alive" );
    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::AbstractVolume>::Getter cbOut =
        std::bind( &VolumeComponent::getVolumeOutput, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Geometry::AbstractVolume>(
        getEntity(), this, id, cbOut );

    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::AbstractVolume>::ReadWrite cbRw =
        std::bind( &VolumeComponent::getVolumeRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Geometry::AbstractVolume>(
        getEntity(), this, id, cbRw );

    ComponentMessenger::CallbackTypes<Index>::Getter roOut =
        std::bind( &VolumeComponent::roIndexRead, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Utils::Index>(
        getEntity(), this, id, roOut );
}

const Index* VolumeComponent::roIndexRead() const {
    CORE_ASSERT( m_displayVolume != nullptr,
                 "DisplayVolume should exist while component is alive" );
    return &m_volumeIndex;
}

const Ra::Core::Geometry::AbstractVolume* VolumeComponent::getVolumeOutput() const {
    return &m_displayVolume->getVolume();
}

Ra::Core::Geometry::AbstractVolume* VolumeComponent::getVolumeRw() {
    return &m_displayVolume->getVolume();
}

} // namespace Engine
} // namespace Ra
