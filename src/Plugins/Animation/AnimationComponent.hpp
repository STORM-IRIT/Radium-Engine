#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <Engine/Entity/Component.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Animation.hpp>
#include <Plugins/FancyMesh/FancyMeshComponent.hpp>

namespace AnimationPlugin
{

class SkeletonBoneRenderObject;

class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component
{
public:
    AnimationComponent(const std::string& name) : Component(name) {}
    virtual ~AnimationComponent() {}

    virtual void initialize() override;
    void set(const Ra::Core::Animation::Skeleton& skel);
    void update(Scalar dt);
    void handleLoading(const AnimationLoader::AnimationData& data);

    Ra::Core::Animation::Skeleton& getSkeleton() { return m_skel; }

    //
    // Editable interface
    //

    virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override;
    virtual void setProperty( const Ra::Engine::EditableProperty& prop) override;
    
    void setMeshComponent(FancyMeshPlugin::FancyMeshComponent* component);
    FancyMeshPlugin::FancyMeshComponent* getMeshComponent() const;
    Ra::Core::Animation::Animation getAnimation() const;
    Ra::Core::Animation::WeightMatrix getWeights() const;
    Ra::Core::Animation::Pose getRefPose() const;

protected:
    Ra::Core::Animation::Skeleton m_skel;
    Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.
    Ra::Core::Animation::Animation m_animation;
    Ra::Core::Animation::WeightMatrix m_weights;
    
    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;
	Scalar m_animationTime;
    FancyMeshPlugin::FancyMeshComponent* m_meshComponent;
};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
