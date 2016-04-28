#include <FancyMeshComponent.hpp>

#include <iostream>

#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Core/Geometry/Normal/Normal.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/GeometryData.hpp>

using Ra::Core::TriangleMesh;
using Ra::Engine::ComponentMessenger;

typedef Ra::Core::VectorArray<Ra::Core::Triangle> TriangleArray;

namespace FancyMeshPlugin
{
    FancyMeshComponent::FancyMeshComponent(const std::string& name , bool deformable)
        : Ra::Engine::Component( name  ) , m_deformable(deformable)
    {
    }

    FancyMeshComponent::~FancyMeshComponent()
    {
    }

    void FancyMeshComponent::initialize()
    {
    }

    void FancyMeshComponent::addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name )
    {
        setupIO(name);

        std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );
        displayMesh->loadGeometry( mesh );

        auto renderObject = Ra::Engine::RenderObject::createRenderObject(name, this, Ra::Engine::RenderObjectType::Fancy, displayMesh);
        addRenderObject(renderObject);
    }

    void FancyMeshComponent::handleMeshLoading( const Ra::Asset::GeometryData* data )
    {
        std::string name( m_name );
        name.append( "_" + data->getName() );

        std::string roName = name;
        roName.append( "_RO" );

        std::string meshName = name;
        meshName.append( "_Mesh" );

        std::string matName = name;
        matName.append( "_Mat" );

        m_contentName = data->getName();

        std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( meshName ) );

        Ra::Core::TriangleMesh mesh;
        Ra::Core::Transform T = data->getFrame();
        Ra::Core::Transform N;
        N.matrix() = (T.matrix()).inverse().transpose();

        for (size_t i = 0; i < data->getVerticesSize(); ++i)
        {
            mesh.m_vertices.push_back(T * data->getVertices()[i]);
            mesh.m_normals.push_back((N * data->getNormals()[i]).normalized());
        }

        for (const auto& face : data->getFaces())
        {
            mesh.m_triangles.push_back(face.head<3>());
        }

        setupIO( data->getName());

        displayMesh->loadGeometry(mesh);

        Ra::Core::Vector3Array tangents;
        Ra::Core::Vector3Array bitangents;
        Ra::Core::Vector3Array texcoords;

        Ra::Core::Vector4Array colors;

        for ( const auto& v : data->getTangents() )     tangents.push_back( v );
        for ( const auto& v : data->getBiTangents() )   bitangents.push_back( v );
        for ( const auto& v : data->getTexCoords() )    texcoords.push_back( v );
        for ( const auto& v : data->getColors() )       colors.push_back( v );

        displayMesh->addData( Ra::Engine::Mesh::VERTEX_TANGENT, tangents );
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_BITANGENT, bitangents );
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_TEXCOORD, texcoords );
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_COLOR, colors );

        // FIXME(Charly): Should not weights be part of the geometry ?
        //        mesh->addData( Ra::Engine::Mesh::VERTEX_WEIGHTS, meshData.weights );

        Ra::Engine::Material* mat = new Ra::Engine::Material( matName );
        auto m = data->getMaterial();
        if ( m.hasDiffuse() )   mat->setKd( m.m_diffuse );
        if ( m.hasSpecular() )  mat->setKs( m.m_specular );
        if ( m.hasShininess() ) mat->setNs( m.m_shininess );
#ifdef LOAD_TEXTURES
        if ( m.hasDiffuseTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_DIFFUSE, m.m_texDiffuse );
#endif
        //if ( m.hasSpecularTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_SPECULAR, m.m_texSpecular );
        //if ( m.hasShininessTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_SHININESS, m.m_texShininess );
        //if ( m.hasOpacityTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_ALPHA, m.m_texOpacity );
        //if ( m.hasNormalTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_NORMAL, m.m_texNormal );

        auto config = Ra::Engine::ShaderConfigurationFactory::getConfiguration("BlinnPhong");

        Ra::Engine::RenderObject* renderObject = Ra::Engine::RenderObject::createRenderObject(roName, this, Ra::Engine::RenderObjectType::Fancy, displayMesh, config, mat);
        m_meshIndex = addRenderObject(renderObject);
    }

    Ra::Core::Index FancyMeshComponent::getRenderObjectIndex() const
    {
        return m_meshIndex;
    }

    const Ra::Core::TriangleMesh& FancyMeshComponent::getMesh() const
    {
        return getDisplayMesh().getGeometry();
    }

    void FancyMeshComponent::setupIO(const std::string& id)
    {
        ComponentMessenger::CallbackTypes<TriangleMesh>::Getter cbOut = std::bind( &FancyMeshComponent::getMeshOutput, this );
        ComponentMessenger::getInstance()->registerOutput<TriangleMesh>( getEntity(), this, id, cbOut);

        if( m_deformable)
        {
            ComponentMessenger::CallbackTypes<TriangleMesh>::Setter cbIn = std::bind( &FancyMeshComponent::setMeshInput, this, std::placeholders::_1 );
            ComponentMessenger::getInstance()->registerInput<TriangleMesh>( getEntity(), this, id, cbIn);

            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite vRW = std::bind( &FancyMeshComponent::getVerticesRw, this);
            ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this, id+"v", vRW);

            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite nRW = std::bind( &FancyMeshComponent::getNormalsRw, this);
            ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this, id+"n", nRW);

            ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite tRW = std::bind( &FancyMeshComponent::getTrianglesRw, this);
            ComponentMessenger::getInstance()->registerReadWrite<TriangleArray>( getEntity(), this, id+"t", tRW);
        }

    }

    const Ra::Engine::Mesh& FancyMeshComponent::getDisplayMesh() const
    {
        return *(getRoMgr()->getRenderObject(getRenderObjectIndex())->getMesh());
    }

    Ra::Engine::Mesh& FancyMeshComponent::getDisplayMesh()
    {
        return *(getRoMgr()->getRenderObject(getRenderObjectIndex())->getMesh());
    }

    const Ra::Core::TriangleMesh* FancyMeshComponent::getMeshOutput() const
    {
        return &(getMesh());
    }

    void FancyMeshComponent::setMeshInput(const TriangleMesh *meshptr)
    {
        CORE_ASSERT( meshptr, " Input is null");
        CORE_ASSERT( m_deformable, "Mesh is not deformable");

        Ra::Engine::Mesh& displayMesh = getDisplayMesh();
        displayMesh.loadGeometry( *meshptr );
    }

    Ra::Core::Vector3Array* FancyMeshComponent::getVerticesRw()
    {
        getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_POSITION);
        return &(getDisplayMesh().getGeometry().m_vertices);
    }

    Ra::Core::Vector3Array* FancyMeshComponent::getNormalsRw()
    {
        getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_NORMAL);
        return &(getDisplayMesh().getGeometry().m_normals);
    }

    Ra::Core::VectorArray<Ra::Core::Triangle>* FancyMeshComponent::getTrianglesRw()
    {
        getDisplayMesh().setDirty( Ra::Engine::Mesh::INDEX);
        return &(getDisplayMesh().getGeometry().m_triangles);
    }

    void FancyMeshComponent::rayCastQuery( const Ra::Core::Ray& r) const
    {
        auto result  = Ra::Core::MeshUtils::castRay( getMesh(), r );
        int tidx = result.m_hitTriangle;
        if (tidx >= 0)
        {
            LOG(logINFO) << " Hit triangle " << tidx;
            LOG(logINFO) << " Nearest vertex " << result.m_nearestVertex;
        }
    }

} // namespace FancyMeshPlugin
