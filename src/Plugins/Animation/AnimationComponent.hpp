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

class AnimationComponent : public Ra::Engine::Component
{
public:
    AnimationComponent(const std::string& name) : Component(name), m_selectedBone(-1) {}
    virtual ~AnimationComponent() {}

    virtual void initialize() override;
    void set(const Ra::Core::Animation::Skeleton& skel);
    void update(Scalar dt);
    void handleLoading(const AnimationLoader::AnimationData& data);
    void reset();

    inline Ra::Core::Animation::Skeleton& getSkeleton() { return m_skel; }

    //
    // Editable interface
    //

    virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override;
    virtual void setProperty( const Ra::Engine::EditableProperty& prop) override;
    virtual bool picked (uint drawableIdex) const override;

    ANIM_PLUGIN_API void setMeshComponent(FancyMeshPlugin::FancyMeshComponent* component);
    ANIM_PLUGIN_API FancyMeshPlugin::FancyMeshComponent* getMeshComponent() const;
    ANIM_PLUGIN_API Ra::Core::Animation::WeightMatrix getWeights() const;
    ANIM_PLUGIN_API Ra::Core::Animation::Pose getRefPose() const;

protected:
    // debug function
    void printSkeleton(const Ra::Core::Animation::Skeleton& skeleton);

protected:
    Ra::Core::Animation::Skeleton m_skel;
    Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.
    std::vector<Ra::Core::Animation::Animation> m_animations;
    Ra::Core::Animation::WeightMatrix m_weights;

    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;
    Scalar m_animationTime;
    FancyMeshPlugin::FancyMeshComponent* m_meshComponent;
    mutable int m_selectedBone; //this is an ugly hack ! (Val)
};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
