#include <FancyMeshComponent.hpp>

#include <iostream>

#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/GeometryData.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>


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

        auto displayMesh = Ra::Core::make_shared<Ra::Engine::Mesh>(meshName/*, Ra::Engine::Mesh::RM_POINTS*/);

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

        displayMesh->loadGeometry(mesh);

        if (data->hasTangents())
        {
            Ra::Core::Vector3Array tangents;   tangents.resize(data->getTangents().size());
            std::copy(data->getTangents().begin(),   data->getTangents().end(),   tangents.begin());
            displayMesh->addData( Ra::Engine::Mesh::VERTEX_TANGENT, tangents );
        }

        if (data->hasBiTangents())
        {
            Ra::Core::Vector3Array bitangents; bitangents.resize(data->getBiTangents().size());
            std::copy(data->getBiTangents().begin(), data->getBiTangents().end(), bitangents.begin());
            displayMesh->addData( Ra::Engine::Mesh::VERTEX_BITANGENT, bitangents );
        }

        if (data->hasTextureCoordinates())
        {
            Ra::Core::Vector3Array texcoords;  texcoords.resize(data->getTexCoords().size());
            std::copy(data->getTexCoords().begin(),  data->getTexCoords().end(),  texcoords.begin());
            displayMesh->addData( Ra::Engine::Mesh::VERTEX_TEXCOORD, texcoords );
        }

        if (data->hasColors())
        {
            Ra::Core::Vector4Array colors;     colors.resize(data->getColors().size());
            std::copy(data->getColors().begin(),     data->getColors().end(),     colors.begin());
            displayMesh->addData( Ra::Engine::Mesh::VERTEX_COLOR, colors );
        }

        // FIXME(Charly): Should not weights be part of the geometry ?
        //        mesh->addData( Ra::Engine::Mesh::VERTEX_WEIGHTS, meshData.weights );

        std::shared_ptr<Ra::Engine::Material> mat (new Ra::Engine::Material( matName ));
        auto m = data->getMaterial();
        if ( m.hasDiffuse() )   mat->m_kd    = m.m_diffuse;
        if ( m.hasSpecular() )  mat->m_ks    = m.m_specular;
        if ( m.hasShininess() ) mat->m_ns    = m.m_shininess;
        if ( m.hasOpacity() )   mat->m_alpha = m.m_opacity;

#ifdef RADIUM_WITH_TEXTURES
        if ( m.hasDiffuseTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_DIFFUSE, m.m_texDiffuse );
        if ( m.hasSpecularTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_SPECULAR, m.m_texSpecular );
        if ( m.hasShininessTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_SHININESS, m.m_texShininess );
        if ( m.hasOpacityTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_ALPHA, m.m_texOpacity );
        if ( m.hasNormalTexture() ) mat->addTexture( Ra::Engine::Material::TextureType::TEX_NORMAL, m.m_texNormal );
#endif

        auto config = Ra::Engine::ShaderConfigurationFactory::getConfiguration("BlinnPhong");

        auto ro = Ra::Engine::RenderObject::createRenderObject(roName, this, Ra::Engine::RenderObjectType::Fancy, displayMesh, config, mat);
        if ( mat->m_alpha < 1.0 ) ro->setTransparent(true);

        setupIO( data->getName());
        m_meshIndex = addRenderObject(ro);
    }

    Ra::Core::Index FancyMeshComponent::getRenderObjectIndex() const
    {
        return m_meshIndex;
    }

    const Ra::Core::TriangleMesh& FancyMeshComponent::getMesh() const
    {
        return getDisplayMesh().getGeometry();
    }

    void FancyMeshComponent::setDeformable (const bool b)
    {
        this->m_deformable=b;
    }

    void FancyMeshComponent::setContentName (const std::string name)
    {
        this->m_contentName=name;
    }

    void FancyMeshComponent::setupIO(const std::string& id)
    {
        auto msg = ComponentMessenger::getInstance();

        ComponentMessenger::CallbackTypes<TriangleMesh>::Getter cbOut = std::bind( &FancyMeshComponent::getMeshOutput, this );
        msg->registerOutput<TriangleMesh>( getEntity(), this, id, cbOut);

        ComponentMessenger::CallbackTypes<TriangleMesh>::ReadWrite cbRw = std::bind( &FancyMeshComponent::getMeshRw, this );
        msg->registerReadWrite<TriangleMesh>( getEntity(), this, id, cbRw);

        ComponentMessenger::CallbackTypes<Ra::Core::Index>::Getter roOut = std::bind(&FancyMeshComponent::roIndexRead, this);
        msg->registerOutput<Ra::Core::Index>(getEntity(), this, id, roOut);

        if( m_deformable)
        {
            ComponentMessenger::CallbackTypes<TriangleMesh>::Setter cbIn = std::bind( &FancyMeshComponent::setMeshInput, this, std::placeholders::_1 );
            msg->registerInput<TriangleMesh>( getEntity(), this, id, cbIn);

            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite vRW = std::bind( &FancyMeshComponent::getVerticesRw, this);
            msg->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this, id+"v", vRW);

            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite nRW = std::bind( &FancyMeshComponent::getNormalsRw, this);
            msg->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this, id+"n", nRW);

            ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite tRW = std::bind( &FancyMeshComponent::getTrianglesRw, this);
            msg->registerReadWrite<TriangleArray>( getEntity(), this, id+"t", tRW);
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

    Ra::Core::TriangleMesh *FancyMeshComponent::getMeshRw()
    {
        getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_POSITION );
        getDisplayMesh().setDirty( Ra::Engine::Mesh::VERTEX_NORMAL );
        getDisplayMesh().setDirty( Ra::Engine::Mesh::INDEX);
        return &(getDisplayMesh().getGeometry());
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

    const Ra::Core::Index* FancyMeshComponent::roIndexRead() const
    {
        return &m_meshIndex;
    }

} // namespace FancyMeshPlugin
