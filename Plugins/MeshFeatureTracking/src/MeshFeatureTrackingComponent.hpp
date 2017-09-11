#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_

#include <MeshFeatureTrackingPluginMacros.hpp>

#include <Engine/Component/Component.hpp>
#include <GuiBase/Utils/FeaturePickingManager.hpp>

namespace MeshFeatureTrackingPlugin
{
    class MeshFeatureTrackingComponent : public Ra::Engine::Component
    {
    public:
        MeshFeatureTrackingComponent(const std::string& name);

        virtual ~MeshFeatureTrackingComponent();

        virtual void initialize() override;

        void setData( const Ra::Gui::FeatureData& data );
        void update();

        Ra::Core::Vector3 getFeatureVector() const;
        Ra::Core::Vector3 getFeaturePosition() const;
        Scalar getFeatureScale() const;

        /// Registers the new center for the sphere.
        void setPosition( Ra::Core::Vector3 position );
        /// Registers the new scale for the sphere.
        void setScale( Scalar scale );

        Ra::Gui::FeatureData m_data;
        Ra::Engine::RenderObject *m_RO;
    };
}

#endif //MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
