#include <FancyMeshComponent.hpp>

#include <iostream>
#include <numeric> // std::iota

#include <Core/Containers/MakeShared.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/GeometryData.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>

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

using Ra::Core::TriangleMesh;
using Ra::Engine::ComponentMessenger;

using TriangleArray = Ra::Core::VectorArray<Ra::Core::Triangle>;

namespace FancyMeshPlugin {
FancyMeshComponent::FancyMeshComponent( const std::string& name, bool deformable,
                                        Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( name, entity ),
    m_deformable( deformable ) {}

FancyMeshComponent::~FancyMeshComponent() {}

void FancyMeshComponent::initialize() {}

void FancyMeshComponent::addMeshRenderObject( const Ra::Core::TriangleMesh& mesh,
                                              const std::string& name ) {
    setupIO( name );

    std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );
    displayMesh->loadGeometry( mesh );

    auto renderObject = Ra::Engine::RenderObject::createRenderObject(
        name, this, Ra::Engine::RenderObjectType::Fancy, displayMesh );
    m_meshIndex = addRenderObject( renderObject );
}

void FancyMeshComponent::handleMeshLoading( const Ra::Asset::GeometryData* data ) {
    std::string name( m_name );
    name.append( "_" + data->getName() );

    std::string roName = name;

    roName.append( "_RO" );
    std::string meshName = name;
    meshName.append( "_Mesh" );

    std::string matName = name;
    matName.append( "_Mat" );

    m_contentName = data->getName();

    m_duplicateTable = data->getDuplicateTable();

    auto displayMesh =
        Ra::Core::make_shared<Ra::Engine::Mesh>( meshName /*, Ra::Engine::Mesh::RM_POINTS*/ );

    Ra::Core::TriangleMesh mesh;
    Ra::Core::Transform T = data->getFrame();
    Ra::Core::Transform N;
    N.matrix() = ( T.matrix() ).inverse().transpose();

    mesh.vertices().resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );
#pragma omp parallel for
    for ( uint i = 0; i < data->getVerticesSize(); ++i )
    {
        mesh.vertices()[i] = T * data->getVertices()[i];
    }

    if ( data->hasNormals() )
    {
        mesh.normals().resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );
#pragma omp parallel for
        for ( uint i = 0; i < data->getVerticesSize(); ++i )
        {
            mesh.normals()[i] = ( N * data->getNormals()[i] ).normalized();
        }
    }

    mesh.m_triangles.resize( data->getFaces().size(), Ra::Core::Triangle::Zero() );
#pragma omp parallel for
    for ( uint i = 0; i < data->getFaces().size(); ++i )
    {
        mesh.m_triangles[i] = data->getFaces()[i].head<3>();
    }

    displayMesh->loadGeometry( mesh );

    // get the actual duplicate table according to the mesh, not to the file data.
    if ( !data->isLoadingDuplicates() )
    {
        m_duplicateTable.resize( data->getVerticesSize() );
        std::iota( m_duplicateTable.begin(), m_duplicateTable.end(), 0 );
    } else
    { Ra::Core::MeshUtils::findDuplicates( mesh, m_duplicateTable ); }

    if ( data->hasTangents() )
    {
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_TANGENT, data->getTangents() );
    }

    if ( data->hasBiTangents() )
    {
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_BITANGENT, data->getBiTangents() );
    }

    if ( data->hasTextureCoordinates() )
    {
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_TEXCOORD, data->getTexCoords() );
    }

    if ( data->hasColors() )
    {
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_COLOR, data->getColors() );
    }

    // FIXME(Charly): Should not weights be part of the geometry ?
    //        mesh->addData( Ra::Engine::Mesh::VERTEX_WEIGHTS, meshData.weights );

    // The technique for rendering this component
    Ra::Engine::RenderTechnique rt;

    bool isTransparent{false};
    if ( data->hasMaterial() )
    {
        const Ra::Asset::MaterialData& loadedMaterial = data->getMaterial();

        // First extract the material from asset
        auto converter =
            Ra::Engine::EngineMaterialConverters::getMaterialConverter( loadedMaterial.getType() );
        auto convertedMaterial = converter.second( &loadedMaterial );

        // Second, associate the material to the render technique
        std::shared_ptr<Ra::Engine::Material> radiumMaterial( convertedMaterial );
        if ( radiumMaterial != nullptr )
        {
            isTransparent = radiumMaterial->isTransparent();
        }
        rt.setMaterial( radiumMaterial );

        // Third, define the technique for rendering this material (here, using the default)
        auto builder =
            Ra::Engine::EngineRenderTechniques::getDefaultTechnique( loadedMaterial.getType() );
        builder.second( rt, isTransparent );
    } else
    {
        auto mat = Ra::Core::make_shared<Ra::Engine::BlinnPhongMaterial>( data->getName() +
                                                                          "_DefaulBPMaterial" );
        mat->m_kd = Ra::Core::Colors::Grey();
        mat->m_ks = Ra::Core::Colors::White();
        rt.setMaterial( mat );
        auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( rt, isTransparent );
    }

    auto ro = Ra::Engine::RenderObject::createRenderObject(
        roName, this, Ra::Engine::RenderObjectType::Fancy, displayMesh, rt );
    ro->setTransparent( isTransparent );

    setupIO( m_contentName );
    m_meshIndex = addRenderObject( ro );
}

Ra::Core::Index FancyMeshComponent::getRenderObjectIndex() const {
    return m_meshIndex;
}

const Ra::Core::TriangleMesh& FancyMeshComponent::getMesh() const {
    return getDisplayMesh().getGeometry();
}

void FancyMeshComponent::setDeformable( const bool b ) {
    this->m_deformable = b;
}

void FancyMeshComponent::setContentName( const std::string name ) {
    this->m_contentName = name;
}

void FancyMeshComponent::setupIO( const std::string& id ) {
    ComponentMessenger::CallbackTypes<TriangleMesh>::Getter cbOut =
        std::bind( &FancyMeshComponent::getMeshOutput, this );
    ComponentMessenger::getInstance()->registerOutput<TriangleMesh>( getEntity(), this, id, cbOut );

    ComponentMessenger::CallbackTypes<FancyMeshComponent::DuplicateTable>::Getter dtOut =
        std::bind( &FancyMeshComponent::getDuplicateTableOutput, this );
    ComponentMessenger::getInstance()->registerOutput<FancyMeshComponent::DuplicateTable>(
        getEntity(), this, id, dtOut );

    ComponentMessenger::CallbackTypes<TriangleMesh>::ReadWrite cbRw =
        std::bind( &FancyMeshComponent::getMeshRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<TriangleMesh>( getEntity(), this, id,
                                                                        cbRw );

    ComponentMessenger::CallbackTypes<Ra::Core::Index>::Getter roOut =
        std::bind( &FancyMeshComponent::roIndexRead, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Index>( getEntity(), this, id,
                                                                        roOut );

    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite vRW =
        std::bind( &FancyMeshComponent::getVerticesRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this,
                                                                                  id + "v", vRW );

    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite nRW =
        std::bind( &FancyMeshComponent::getNormalsRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this,
                                                                                  id + "n", nRW );

    ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite tRW =
        std::bind( &FancyMeshComponent::getTrianglesRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<TriangleArray>( getEntity(), this,
                                                                         id + "t", tRW );

    if ( m_deformable )
    {
        ComponentMessenger::CallbackTypes<TriangleMesh>::Setter cbIn =
            std::bind( &FancyMeshComponent::setMeshInput, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<TriangleMesh>( getEntity(), this, id,
                                                                        cbIn );
    }
}

const Ra::Engine::Mesh& FancyMeshComponent::getDisplayMesh() const {
    return *( getRoMgr()->getRenderObject( getRenderObjectIndex() )->getMesh() );
}

Ra::Engine::Mesh& FancyMeshComponent::getDisplayMesh() {
    return *( getRoMgr()->getRenderObject( getRenderObjectIndex() )->getMesh() );
}

const Ra::Core::TriangleMesh* FancyMeshComponent::getMeshOutput() const {
    return &( getMesh() );
}

const FancyMeshComponent::DuplicateTable* FancyMeshComponent::getDuplicateTableOutput() const {
    return &m_duplicateTable;
}

Ra::Core::TriangleMesh* FancyMeshComponent::getMeshRw() {
    getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_POSITION );
    getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_NORMAL );
    getDisplayMesh().setDirty( Ra::Engine::Mesh::INDEX );
    return &( getDisplayMesh().getGeometry() );
}

void FancyMeshComponent::setMeshInput( const TriangleMesh* meshptr ) {
    CORE_ASSERT( meshptr, " Input is null" );
    CORE_ASSERT( m_deformable, "Mesh is not deformable" );

    Ra::Engine::Mesh& displayMesh = getDisplayMesh();
    displayMesh.loadGeometry( *meshptr );
}

Ra::Core::TriangleMesh::PointAttribHandle::Container* FancyMeshComponent::getVerticesRw() {
    getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_POSITION );
    return &( getDisplayMesh().getGeometry().vertices() );
}

Ra::Core::TriangleMesh::NormalAttribHandle::Container* FancyMeshComponent::getNormalsRw() {
    getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_NORMAL );
    return &( getDisplayMesh().getGeometry().normals() );
}

Ra::Core::VectorArray<Ra::Core::Triangle>* FancyMeshComponent::getTrianglesRw() {
    getDisplayMesh().setDirty( Ra::Engine::Mesh::INDEX );
    return &( getDisplayMesh().getGeometry().m_triangles );
}

const Ra::Core::Index* FancyMeshComponent::roIndexRead() const {
    return &m_meshIndex;
}

} // namespace FancyMeshPlugin
