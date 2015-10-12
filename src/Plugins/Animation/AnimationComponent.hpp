#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/Component.hpp>

#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>

namespace AnimationPlugin
{

class SkeletonBoneRenderObject;

class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component 
{
public:
    AnimationComponent(const std::string& name) : Component(name) {}
    AnimationComponent(const std::string& name, Ra::Core::Animation::Skeleton& skel, const Ra::Core::Animation::RefPose& refPose )
            : Component(name),m_skel(skel), m_refPose(refPose)
    {
    }
    virtual ~AnimationComponent() {}

    virtual void initialize() override;
    void set(const Ra::Core::Animation::Skeleton& skel);
	void handleLoading(const AnimationLoader::AnimationData& data);
	


    const Ra::Core::Animation::Skeleton& getSkeleton() const { return m_skel; }

    //
    // Editable interface
    //

    void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override;


protected:
    Ra::Core::Animation::Skeleton m_skel;
    Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.

    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;

};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
