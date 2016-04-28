#ifndef SKINPLUGIN_SKINNING_COMPONENT_HPP_
#define SKINPLUGIN_SKINNING_COMPONENT_HPP_

#include <SkinningPlugin.hpp>

#include <Core/Math/DualQuaternion.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Skinning/SkinningData.hpp>

#include <Engine/Assets/HandleData.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>


namespace SkinningPlugin
{
    class SKIN_PLUGIN_API SkinningComponent : public Ra::Engine::Component
    {
    public:
        SkinningComponent( const std::string& name) : Component(name), m_isReady(false) {}
        virtual ~SkinningComponent() {}

        virtual void initialize() override { setupSkinning();}

        void skin();
        void endSkinning();
        void setupSkinning();

        virtual void handleWeightsLoading( const Ra::Asset::HandleData* data );

        virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override {}
        virtual void setProperty(const Ra::Engine::EditableProperty &newProp) override {}

        const Ra::Core::Skinning::RefData* getRefData() const { return &m_refData;}
        const Ra::Core::Skinning::FrameData* getFrameData() const { return &m_frameData;}
        const Ra::Core::AlignedStdVector< Ra::Core::DualQuaternion >* getDQ() const {return &m_DQ;}

    private:
        void setupIO(const std::string &id);

    private:

            std::string m_contentsName;

            // Skinning data
            Ra::Core::Skinning::RefData m_refData;
            Ra::Core::Skinning::FrameData m_frameData;

            Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Animation::Skeleton>::Getter m_skeletonGetter;

            Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite m_verticesWriter;
            Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite m_normalsWriter;

            Ra::Core::AlignedStdVector< Ra::Core::DualQuaternion > m_DQ;

            bool m_isReady;
    };
}

#endif //  SKINPLUGIN_SKINNING_COMPONENT_HPP_

