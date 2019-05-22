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

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

using TriangleArray = Ra::Core::VectorArray<Ra::Core::Vector3ui>;

namespace Ra {
namespace Engine {
TriangleMeshComponent::TriangleMeshComponent( const std::string& name,
                                              Entity* entity,
                                              const Ra::Core::Asset::GeometryData* data ) :
    Component( name, entity ),
    m_displayMesh( nullptr ) {
    generateTriangleMesh( data );
}

TriangleMeshComponent::TriangleMeshComponent( const std::string& name,
                                              Entity* entity,
                                              Core::Geometry::TriangleMesh&& mesh,
                                              Core::Asset::MaterialData* mat ) :
    Component( name, entity ),
    m_contentName( name ),
    m_displayMesh( new Engine::Mesh( name ) ) {
    m_displayMesh->loadGeometry( std::move( mesh ) );
    finalizeROFromGeometry( mat );
}

//////////// STORE Mesh/TriangleMesh here instead of an index, so don't need to request the ROMgr
/// and no problem with the Displayable -> doesn't affect the API

TriangleMeshComponent::~TriangleMeshComponent() = default;

void TriangleMeshComponent::initialize() {}

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
    RenderTechnique rt;

    bool isTransparent{false};

    if ( data != nullptr )
    {
        // First extract the material from asset
        auto converter         = EngineMaterialConverters::getMaterialConverter( data->getType() );
        auto convertedMaterial = converter.second( data );

        // Second, associate the material to the render technique
        std::shared_ptr<Material> radiumMaterial( convertedMaterial );
        if ( radiumMaterial != nullptr ) { isTransparent = radiumMaterial->isTransparent(); }
        rt.setMaterial( radiumMaterial );

        // Third, define the technique for rendering this material (here, using the default)
        auto builder = EngineRenderTechniques::getDefaultTechnique( data->getType() );
        builder.second( rt, isTransparent );
    }
    else
    {
        auto mat =
            Ra::Core::make_shared<BlinnPhongMaterial>( m_contentName + "_DefaultBPMaterial" );
        mat->m_kd            = Ra::Core::Utils::Color::Grey();
        mat->m_ks            = Ra::Core::Utils::Color::White();
        mat->m_renderAsSplat = m_displayMesh->getNumFaces() == 0;
        mat->m_hasPerVertexKd =
            m_displayMesh->getCoreGeometry().hasAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ) );
        rt.setMaterial( mat );
        auto builder = EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( rt, false );
    }

    if ( m_displayMesh->getCoreGeometry().m_indices.empty() ) // add geometry shader for splatting
    {
        auto addGeomShader = [&rt]( RenderTechnique::PassName pass ) {
            if ( rt.hasConfiguration( pass ) )
            {
                ShaderConfiguration config = rt.getConfiguration( pass );
                config.addShader( ShaderType_GEOMETRY,
                                  std::string( Core::Resources::getRadiumResourcesDir() ) +
                                      "Shaders/PointCloud.geom.glsl" );
                rt.setConfiguration( config, pass );
            }
        };

        addGeomShader( RenderTechnique::LIGHTING_OPAQUE );
        addGeomShader( RenderTechnique::LIGHTING_TRANSPARENT );
        addGeomShader( RenderTechnique::Z_PREPASS );
    }

    std::string roName( m_name + "_" + m_contentName + "_RO" );

    auto ro = RenderObject::createRenderObject(
        roName, this, RenderObjectType::Geometry, m_displayMesh, rt );
    ro->setTransparent( isTransparent );

    setupIO( m_contentName );
    m_meshIndex = addRenderObject( ro );
}

Ra::Core::Utils::Index TriangleMeshComponent::getRenderObjectIndex() const {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    return m_meshIndex;
}

const Ra::Core::Geometry::TriangleMesh& TriangleMeshComponent::getMesh() const {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    return m_displayMesh->getCoreGeometry();
}

Mesh* TriangleMeshComponent::getDisplayMesh() {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    return m_displayMesh.get();
}

void TriangleMeshComponent::setContentName( const std::string& name ) {
    this->m_contentName = name;
}

void TriangleMeshComponent::setupIO( const std::string& id ) {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::Getter cbOut =
        std::bind( &TriangleMeshComponent::getMeshOutput, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Geometry::TriangleMesh>(
        getEntity(), this, id, cbOut );

    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::ReadWrite cbRw =
        std::bind( &TriangleMeshComponent::getMeshRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Geometry::TriangleMesh>(
        getEntity(), this, id, cbRw );

    ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter roOut =
        std::bind( &TriangleMeshComponent::roIndexRead, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Utils::Index>(
        getEntity(), this, id, roOut );
}

const Ra::Core::Geometry::TriangleMesh* TriangleMeshComponent::getMeshOutput() const {
    return &m_displayMesh->getCoreGeometry();
}

Ra::Core::Geometry::TriangleMesh* TriangleMeshComponent::getMeshRw() {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    return &( m_displayMesh->getCoreGeometry() );
}

const Ra::Core::Utils::Index* TriangleMeshComponent::roIndexRead() const {
    CORE_ASSERT( m_displayMesh != nullptr, "DisplayMesh should exist while component is alive" );
    return &m_meshIndex;
}

} // namespace Engine
} // namespace Ra
