#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include "AnimationPlugin.hpp"

#include <Engine/Entity/Component.hpp>
#include <Plugins/Animation/AnimationLoader.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Animation.hpp>

#include "AnimationLoader.hpp"

#include <Engine/Assets/HandleData.hpp>
#include <Engine/Assets/AnimationData.hpp>

namespace AnimationPlugin
{

class SkeletonBoneRenderObject;

class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component
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

#define TRY_DEBUG_HANDLES
#ifdef TRY_DEBUG_HANDLES
    void handleSkeletonLoading( const Ra::Asset::HandleData* data, const std::map< uint, uint >& duplicateTable );
    void handleAnimationLoading( const std::vector< Ra::Asset::AnimationData* > data );

    void createSkeleton( const Ra::Asset::HandleData* data, std::map< uint, uint >& indexTable );
    void addBone( const int parent,
                  const uint dataID,
                  const Ra::Core::AlignedStdVector< Ra::Asset::HandleComponentData >& data,
                  const Ra::Core::AlignedStdVector< Ra::Core::Vector2i >& edgeList,
                  std::vector< bool >& processed,
                  std::map< uint, uint >& indexTable );
    void createWeightMatrix( const Ra::Asset::HandleData* data, const std::map< uint, uint >& indexTable, const std::map< uint, uint >& duplicateTable );
#endif
    //
    // Editable interface
    //

    virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override;
    virtual void setProperty( const Ra::Engine::EditableProperty& prop) override;
    virtual bool picked (uint drawableIdex) const override;

    Ra::Core::Animation::WeightMatrix getWeights() const;
    Ra::Core::Animation::Pose getRefPose() const;

protected:
    Ra::Core::Animation::Skeleton m_skel;
    Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.
    std::vector<Ra::Core::Animation::Animation> m_animations;
    Ra::Core::Animation::WeightMatrix m_weights;

    std::vector<SkeletonBoneRenderObject*> m_boneDrawables;
    Scalar m_animationTime;
    mutable int m_selectedBone; //this is an ugly hack ! (Val)
};

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
