#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_

#include <MeshFeatureTrackingPluginMacros.hpp>

#include <Engine/Component/Component.hpp>
#include <GuiBase/Utils/PickingManager.hpp>

namespace MeshFeatureTrackingPlugin {

/// The FeatureData struct stores the feature informations to display.
struct FeatureData {
    /// The feature type: only vertex, edge, triangles are supported.
    Ra::Engine::Renderer::PickingMode m_mode;
    /// The vertices indices in 0-2 according to m_mode, plus triangle idx in 3 for triangles.
    std::array<int, 4> m_data;
};

/**
 * The MeshFeatureTrackingComponent class is a SystemComponent responsible for
 * displaying informations about selected features, and displays a sphere to
 * locate the selected feature.
 */
class MeshFeatureTrackingComponent : public Ra::Engine::Component {
  public:
    MeshFeatureTrackingComponent( const std::string& name );

    ~MeshFeatureTrackingComponent() override;

    void initialize() override;

    /**
     * Sets the Feature data from the picking result.
     */
    void setData( const Ra::Engine::Renderer::PickingResult& data );

    /**
     * Changes the vertex index to track.
     */
    void setVertexIdx( int idx );

    /**
     * Changes the triangle index to track.
     */
    void setTriangleIdx( int idx );

    /**
     * Updates the displayed informations and sphere.
     */
    void update();

    /**
     * Returns the number of vertices available on the object.
     */
    int getMaxV() const;

    /**
     * Returns the number of triangles available on the object.
     */
    int getMaxT() const;

    /**
     * Returns the feature informations.
     */
    FeatureData getFeatureData() const;

    /**
     * Returns the normal of a vertex feature or a triangle feature and
     * the edge vector of an edge feature.
     */
    Ra::Core::Vector3 getFeatureVector() const;

    /**
     * Returns the centroid of the feature, used to display the sphere.
     */
    Ra::Core::Vector3 getFeaturePosition() const;

  private:
    /**
     * Registers the new center for the sphere.
     */
    void setPosition( Ra::Core::Vector3 position );

    /**
     * Registers the new scale for the sphere.
     */
    void setScale( Scalar scale );

    /**
     * Returns the feature scale used to display the sphere.
     */
    Scalar getFeatureScale() const;

  private:
    /// The feature informations.
    FeatureData m_data;

    /// The index of the RenderObject to track feature of.
    int m_pickedRoIdx;

    /// The RenderObject for the sphere.
    Ra::Engine::RenderObject* m_RO;
};

} // namespace MeshFeatureTrackingPlugin

#endif // MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
