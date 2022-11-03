#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API SceneNode : public Node
{
  protected:
    SceneNode( const std::string& instanceName, const std::string& typeName ) :
        Node( instanceName, typeName ) {
        addOutput( m_roOut, m_objects );
        addOutput( m_lightOut, m_lights );
        addOutput( m_cameraOut, m_camera );
    }

  public:
    explicit SceneNode( const std::string& name ) : SceneNode( name, SceneNode::getTypename() ) {}

    bool execute() override {
        auto interfaceRo = static_cast<PortIn<std::vector<RenderObjectPtrType>>*>( m_interface[0] );
        if ( interfaceRo->isLinked() ) { m_roOut->setData( &( interfaceRo->getData() ) ); }
        else {
            m_roOut->setData( m_objects );
        }

        auto interfaceLights = static_cast<PortIn<std::vector<LightPtrType>>*>( m_interface[1] );
        if ( interfaceLights->isLinked() ) {
            m_lightOut->setData( &( interfaceLights->getData() ) );
        }
        else {
            m_lightOut->setData( m_lights );
        }

        auto interfaceCamera = static_cast<PortIn<CameraType>*>( m_interface[2] );
        if ( interfaceCamera->isLinked() ) {
            m_cameraOut->setData( &( interfaceCamera->getData() ) );
        }
        else {
            m_cameraOut->setData( m_camera );
        }
        return true;
    }

    static const std::string getTypename() { return "Scene data provider"; }

    /// Set te scene data collection
    /// @note, these collections must have a lifetime longer than the Scene Node they are associated
    /// to.
    void setScene( std::vector<RenderObjectPtrType>* ros, std::vector<LightPtrType>* lights ) {
        m_objects = ros;
        m_lights  = lights;
    }

    void setCamera( CameraType* camera ) { m_camera = camera; }

  protected:
    bool fromJsonInternal( const nlohmann::json& ) override { return true; }
    void toJsonInternal( nlohmann::json& ) const override {}

  private:
    std::vector<RenderObjectPtrType>* m_objects { nullptr };
    PortOut<std::vector<RenderObjectPtrType>>* m_roOut {
        new PortOut<std::vector<RenderObjectPtrType>>( "objects", this ) };

    std::vector<LightPtrType>* m_lights { nullptr };
    PortOut<std::vector<LightPtrType>>* m_lightOut {
        new PortOut<std::vector<LightPtrType>>( "lights", this ) };

    CameraType* m_camera { nullptr };
    PortOut<CameraType>* m_cameraOut { new PortOut<CameraType>( "camera", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
