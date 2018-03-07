#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_

#include <MeshFeatureTrackingPluginMacros.hpp>

#include <Engine/Component/Component.hpp>
#include <GuiBase/Utils/PickingManager.hpp>

#include <UI/MeshFeatureTrackingUI.h>

namespace MeshFeatureTrackingPlugin {
class MeshFeatureTrackingComponent : public Ra::Engine::Component {
  public:
    MeshFeatureTrackingComponent( const std::string& name );

    virtual ~MeshFeatureTrackingComponent();

    virtual void initialize() override;

    void setData( const Ra::Engine::Renderer::PickingResult& data );

    void setVertexIdx( int idx );
    void setTriangleIdx( int idx );

    void update();

    int getMaxV() const;
    int getMaxT() const;

    FeatureData getFeatureData() const;
    Scalar getFeatureScale() const;
    Ra::Core::Vector3 getFeatureVector() const;
    Ra::Core::Vector3 getFeaturePosition() const;

    /// Registers the new center for the sphere.
    void setPosition( Ra::Core::Vector3 position );
    /// Registers the new scale for the sphere.
    void setScale( Scalar scale );

    FeatureData m_data;
    int m_pickedRoIdx;
    Ra::Engine::RenderObject* m_RO;
};
} // namespace MeshFeatureTrackingPlugin

#endif // MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
