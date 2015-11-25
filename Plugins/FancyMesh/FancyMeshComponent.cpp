#include <Plugins/FancyMesh/FancyMeshComponent.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace FancyMeshPlugin
{
    FancyMeshComponent::FancyMeshComponent( const std::string& name )
        : Ra::Engine::Component( name )
    {
    }

    FancyMeshComponent::~FancyMeshComponent()
    {
        // TODO(Charly): Should we ask the RO manager to delete our render object ?
    }

    void FancyMeshComponent::initialize()
    {
    }

    void FancyMeshComponent::addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name )
    {
        Ra::Engine::RenderTechnique* technique = new Ra::Engine::RenderTechnique;
        technique->material = new Ra::Engine::Material( "Default" );
        technique->shaderConfig = Ra::Engine::ShaderConfiguration( "Default", "../Shaders" );

        addMeshRenderObject(mesh, name, technique);
    }

    void FancyMeshComponent::addMeshRenderObject( const Ra::Core::TriangleMesh& mesh,
                                                  const std::string& name,
                                                  Ra::Engine::RenderTechnique* technique )
    {
        Ra::Engine::RenderObject* renderObject = new Ra::Engine::RenderObject( name, this );
        renderObject->setVisible( true );

        renderObject->setRenderTechnique( technique );

        if ( technique->material->getMaterialType() == Ra::Engine::Material::MaterialType::MAT_TRANSPARENT )
        {
            renderObject->setType( Ra::Engine::RenderObject::Type::RO_TRANSPARENT );
        }

        std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );
        std::vector<uint> indices;
        indices.reserve( mesh.m_triangles.size() * 3 );
        for ( const auto& i : mesh.m_triangles )
        {
            indices.push_back( i.x() );
            indices.push_back( i.y() );
            indices.push_back( i.z() );
        }

        displayMesh->loadGeometry( mesh.m_vertices, indices );
        displayMesh->addData( Ra::Engine::Mesh::VERTEX_NORMAL, mesh.m_normals );

        renderObject->setMesh( displayMesh );

        addRenderObject( renderObject );
    }


    void FancyMeshComponent::handleMeshLoading( const FancyComponentData& data )
    {

        Ra::Engine::RenderObject* renderObject = new Ra::Engine::RenderObject( data.name, this );
        renderObject->setVisible( true );

        FancyMeshData meshData = data.mesh;

        std::stringstream ss;
        ss << data.name << "_mesh_";
        std::string meshName = ss.str();

        std::shared_ptr<Ra::Engine::Mesh> mesh( new Ra::Engine::Mesh( meshName ) );
        
        mesh->loadGeometry( meshData.positions, meshData.indices );

        mesh->addData( Ra::Engine::Mesh::VERTEX_NORMAL, meshData.normals );
        mesh->addData( Ra::Engine::Mesh::VERTEX_TANGENT, meshData.tangents );
        mesh->addData( Ra::Engine::Mesh::VERTEX_BITANGENT, meshData.bitangents );
        mesh->addData( Ra::Engine::Mesh::VERTEX_TEXCOORD, meshData.texcoords );
        mesh->addData( Ra::Engine::Mesh::VERTEX_COLOR, meshData.colors );
        mesh->addData( Ra::Engine::Mesh::VERTEX_WEIGHTS, meshData.weights );

        renderObject->setMesh( mesh );

        m_meshIndex = addRenderObject(renderObject);

        // Build m_mesh
        int triangleCount = meshData.indices.size() / 3;
        int vertexCount = meshData.positions.size();
        m_mesh.m_vertices.resize(vertexCount);
        m_mesh.m_normals.resize(vertexCount);
        m_mesh.m_triangles.resize(triangleCount);

        for (int i = 0; i < vertexCount; i++)
        {
            Ra::Core::Vector4 pos = meshData.positions[i];
            Ra::Core::Vector4 normals = meshData.normals[i];
            m_mesh.m_vertices[i] = Ra::Core::Vector3(pos(0), pos(1), pos(2));
            m_mesh.m_normals[i] = Ra::Core::Vector3(normals(0), normals(1), normals(2));
        }

        for (int i = 0; i < triangleCount; i++)
            m_mesh.m_triangles[i] = Ra::Core::Triangle(meshData.indices[i * 3], meshData.indices[i * 3 + 1], meshData.indices[i * 3 + 2]);

        renderObject->setRenderTechnique( data.renderTechnique );
    }
    
    void FancyMeshComponent::setLoadingInfo(MeshLoadingInfo info)
    {
        m_loadingInfo = info;
    }
    
    const MeshLoadingInfo& FancyMeshComponent::getLoadingInfo() const
    {
        return m_loadingInfo;
    }
    
    Ra::Core::Index FancyMeshComponent::getMeshIndex() const
    {
        return m_meshIndex;
    }
    
    Ra::Core::TriangleMesh FancyMeshComponent::getMesh() const
    {
        return m_mesh;
    }

    void FancyMeshComponent::rayCastQuery( const Ra::Core::Ray& r) const
    {
        auto result  = Ra::Core::MeshUtils::castRay( m_mesh, r );
        int tidx = result.m_hitTriangle;
        if (tidx >= 0)
        {
            LOG(logINFO) << " Hit triangle " << tidx;
            LOG(logINFO) << " Nearest vertex " << result.m_nearestVertex;
        }
    }

} // namespace FancyMeshPlugin
