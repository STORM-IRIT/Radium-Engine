#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_

#include <MeshFeatureTrackingPluginMacros.hpp>

#include <Engine/Component/Component.hpp>
#include <GuiBase/Utils/PickingManager.hpp>

#include <UI/MeshFeatureTrackingUI.h>

namespace MeshFeatureTrackingPlugin {
/// The MeshFeatureTrackingComponent class is a SystemComponent responsible for
/// displaying informations about selected features, and displays a sphere to locate
/// the selected feature.
class MESH_FEATURE_TRACKING_PLUGIN_API MeshFeatureTrackingComponent : public Ra::Engine::Component {
  public:
    MeshFeatureTrackingComponent( const std::string& name );

    virtual ~MeshFeatureTrackingComponent();

    void initialize() override;

    /// Sets the Feature data from the picking result.
    void setData( const Ra::Engine::Renderer::PickingResult& data );

    /// Changes the vertex index to track.
    void setVertexIdx( int idx );

    /// Changes the triangle index to track.
    void setTriangleIdx( int idx );

    /// Updates the displayed informations and sphere.
    void update();

    /// @returns the number of vertices available on the object.
    int getMaxV() const;

    /// @returns the number of triangles available on the object.
    int getMaxT() const;

    /// @returns the feature informations.
    FeatureData getFeatureData() const;

    /// @returns the feature scale used to display the sphere.
    Scalar getFeatureScale() const;

    /// @returns the normal of a vertex feature or a triangle feature and
    /// the edge vector of an edge feature.
    Ra::Core::Vector3 getFeatureVector() const;

    /// @returns the centroid of the feature, used to display the sphere.
    Ra::Core::Vector3 getFeaturePosition() const;

    /// Registers the new center for the sphere.
    void setPosition( Ra::Core::Vector3 position );

    /// Registers the new scale for the sphere.
    void setScale( Scalar scale );

    /// The feature informations.
    FeatureData m_data;

    /// The index of the RenderObject to track feature of.
    int m_pickedRoIdx;

    /// The RenderObject for the sphere.
    Ra::Engine::RenderObject* m_RO;
};
} // namespace MeshFeatureTrackingPlugin

#endif // MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
