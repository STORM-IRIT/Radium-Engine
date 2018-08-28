#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <AnimationPluginMacros.hpp>

#include <Core/Animation/Animation.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/File/AnimationData.hpp>
#include <Core/File/HandleData.hpp>

#include <Engine/Component/Component.hpp>

#include <memory>

namespace AnimationPlugin {

class SkeletonBoneRenderObject;

/// The AnimationComponent is responsible for the management of the animations of an Entity.
/// It stores the animation Skeleton and the animation data as well as the animation playing speed.
class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component {
  public:
    AnimationComponent( const std::string& name, Ra::Engine::Entity* entity );
    virtual ~AnimationComponent();
    AnimationComponent( const AnimationComponent& ) = delete;
    AnimationComponent& operator=( const AnimationComponent& ) = delete;

    void initialize() override {}

    bool canEdit( Ra::Core::Index roIdx ) const override;

    Ra::Core::Transform getTransform( Ra::Core::Index roIdx ) const override;

    void setTransform( Ra::Core::Index roIdx, const Ra::Core::Transform& transform ) override;

    /// Set the animation skeleton.
    void setSkeleton( const Ra::Core::Animation::Skeleton& skel );

    /// @returns the animation skeleton.
    inline Ra::Core::Animation::Skeleton& getSkeleton() { return m_skel; }

    /// @returns the skinning weights.
    Ra::Core::Animation::WeightMatrix getWeights() const;

    /// @returns the reference pose.
    Ra::Core::Animation::Pose getRefPose() const;

    /// Update the skeleton with an animation.
    void update( Scalar dt );

    /// Resets the animation, thus posing the skeleton into the reference pose.
    void reset();

    /// Turns xray display on/off for the skeleton bones.
    void setXray( bool on ) const;

    /// Toggle skeleton bones display.
    void toggleSkeleton( const bool status );

    /// If \p status is TRUE, then use the animation time step if available;
    /// else, use the application timestep.
    void toggleAnimationTimeStep( const bool status );

    /// Set animation speed factor.
    void setSpeed( const Scalar value );

    /// Toggle the slow motion speed (speed x0.1).
    void toggleSlowMotion( const bool status );

    /// Set the animation to play.
    void setAnimation( const uint i );

    /// @returns the index of the skeleton bone associated to the RenderObject with index \p index.
    uint getBoneIdx( Ra::Core::Index index ) const;

    /// @returns the current time of animation.
    Scalar getTime() const;

    /// Create the skeleton from the given data.
    /// @param data: the skeleton's joint transform hierarchy.
    /// @param duplicateTable: the map from duplicate to single vertices.
    /// @param the number of single vertices.
    // FIXME: the 2 last parameters are needed only for the genereation of
    //        the skinning weights matrix.
    void handleSkeletonLoading( const Ra::Asset::HandleData* data,
                                const std::vector<Ra::Core::Index>& duplicateTable,
                                uint nbMeshVertices );

    /// Create the animations from the given data.
    void handleAnimationLoading( const std::vector<Ra::Asset::AnimationData*> data );

  public:
    /// Debug function to display the hierarchy
    void printSkeleton( const Ra::Core::Animation::Skeleton& skeleton );

    /// Sets the skinning weights to use.
    void setWeights( Ra::Core::Animation::WeightMatrix m );

    /// Sets the Entity name for Component communication.
    void setContentName( const std::string name );

    /// Setup Component communication.
    void setupIO( const std::string& id );

    /// Skeleton getter for Component communication.
    const Ra::Core::Animation::Skeleton* getSkeletonOutput() const;

    /// Referene Pose getter for Component communication.
    const Ra::Core::Animation::RefPose* getRefPoseOutput() const;

    /// Skinning Weight Matrix getter for Component communication.
    const Ra::Core::Animation::WeightMatrix* getWeightsOutput() const;

    /// Reset status getter for Component communication.
    const bool* getWasReset() const;

    /// Current Animation getter for Component communication.
    const Ra::Core::Animation::Animation* getAnimation() const;

    /// Current Animation Time for Component communication.
    const Scalar* getTimeOutput() const;

  private:
    // Internal function to create the skinning weights.
    void createWeightMatrix( const Ra::Asset::HandleData* data,
                             const std::map<uint, uint>& indexTable,
                             const std::vector<Ra::Core::Index>& duplicateTable,
                             uint nbMeshVertices );

    // Internal function to create the bone display objects.
    void setupSkeletonDisplay();

  private:
    /// Entity name for Component communication.
    std::string m_contentName;

    /// The Animation Skeleton.
    Ra::Core::Animation::Skeleton m_skel;

    /// The Reference Pose in model space.
    Ra::Core::Animation::RefPose m_refPose;

    /// The animations.
    std::vector<Ra::Core::Animation::Animation> m_animations;

    /// The Skinning Weight Matrix
    // FIXME: this one should go in the SkinningComponent.
    Ra::Core::Animation::WeightMatrix m_weights;

    /// Bones ROs.
    std::vector<std::unique_ptr<SkeletonBoneRenderObject>> m_boneDrawables;

    /// Current animation ID.
    uint m_animationID;

    /// Wheither to use the animation timestep (if available) or the app's.
    bool m_animationTimeStep;

    /// Current animation time.
    Scalar m_animationTime;

    /// Time step of each animation.
    std::vector<Scalar> m_dt;

    /// Animation Play speed.
    Scalar m_speed;

    /// Is slow motion active?
    bool m_slowMo;

    /// Was the animation reset?
    bool m_wasReset;

    /// Is the reset process done?
    bool m_resetDone;
};

} // namespace AnimationPlugin

#endif // ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
