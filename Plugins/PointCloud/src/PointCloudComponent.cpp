#include <PointCloudComponent.hpp>

#include <Core/Mesh/MeshUtils.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/File/FileData.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>


using Ra::Core::TriangleMesh;
using Ra::Engine::ComponentMessenger;

typedef Ra::Core::VectorArray<Ra::Core::Triangle> TriangleArray;

namespace PointCloudPlugin
{
    PointCloudComponent::PointCloudComponent(const std::string& name)
            : Ra::Engine::Component( name  )
    {
    }

    PointCloudComponent::~PointCloudComponent()
    {
    }
/*
    void PointCloudComponent::handleDataLoading( const Ra::Asset::GeometryData* data) {
        std::string name( m_name );
        name.append( "_" + data->getName() );

        m_contentName = data->getName();
        setupIO(m_contentName);
    }
    
    void PointCloudComponent::setupIO(const std::string &id)
    {
        ComponentMessenger::CallbackTypes<bool>::Getter useNormalOut = std::bind( &PointCloudComponent::getUseNormal, this );
        ComponentMessenger::getInstance()->registerOutput<bool>( getEntity(), this, id, useNormalOut);
    }
    
    void PointCloudComponent::setUseNormal(bool useNormal)
    {
        m_useNormal = useNormal;
    }

    const bool* PointCloudComponent::getUseNormal() const
    {
        return &m_useNormal;
    }
*/
    void PointCloudComponent::initialize()
    {
    }

} // namespace PointCloudPlugin
