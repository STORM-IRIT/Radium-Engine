#ifndef SKINPLUGIN_SKINNING_COMPONENT_HPP_
#define SKINPLUGIN_SKINNING_COMPONENT_HPP_

#include <SkinningPlugin.hpp>

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Assets/HandleData.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/ComponentMessenger.hpp>


namespace SkinningPlugin
{
    class SKIN_PLUGIN_API SkinningComponent : public Ra::Engine::Component
    {
    public:
        SkinningComponent( const std::string& name) : Component(name), m_isReady(false) {}
        virtual ~SkinningComponent() {}

        virtual void initialize() override { setupSkinning();}

        void skin();
        void setupSkinning();

        virtual void handleWeightsLoading( const Ra::Asset::HandleData* data );

        virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override {}
        virtual void setProperty(const Ra::Engine::EditableProperty &newProp) override {}
    private:

            std::string m_contentsName;

            // Skinning data

            Ra::Core::TriangleMesh m_referenceMesh;
            Ra::Core::TriangleMesh m_targetMesh;
            Ra::Core::Animation::Pose m_refPose;
            Ra::Core::Animation::WeightMatrix m_weights;

            Ra::Engine::ComponentMessenger::GetterCallback m_skeletonGetter;
            Ra::Engine::ComponentMessenger::SetterCallback m_meshSetter;

            bool m_isReady;
    };
}

#endif //  SKINPLUGIN_SKINNING_COMPONENT_HPP_

