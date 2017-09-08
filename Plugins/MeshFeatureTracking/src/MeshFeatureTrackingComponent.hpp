#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_

#include <MeshFeatureTrackingPluginMacros.hpp>

#include <Engine/Component/Component.hpp>

namespace MeshFeatureTrackingPlugin
{
    class MeshFeatureTrackingComponent : public Ra::Engine::Component
    {
    public:
        MeshFeatureTrackingComponent(const std::string& name) :
            Component(name)
        {}

        virtual ~MeshFeatureTrackingComponent() {}

        virtual void initialize() override {}
    };
}

#endif //MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_COMPONENT_HPP_
