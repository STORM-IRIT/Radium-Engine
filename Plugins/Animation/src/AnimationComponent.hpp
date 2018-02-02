#ifndef ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
#define ANIMPLUGIN_ANIMATION_COMPONENT_HPP_

#include <AnimationPluginMacros.hpp>

#include <Core/Animation/Animation.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/File/AnimationData.hpp>
#include <Core/File/HandleData.hpp>

#include <Engine/Component/Component.hpp>

#include <memory>

namespace AnimationPlugin
{

    class SkeletonBoneRenderObject;

    class ANIM_PLUGIN_API AnimationComponent : public Ra::Engine::Component
    {
    public:
        AnimationComponent(const std::string& name);
        virtual ~AnimationComponent();
        AnimationComponent(const AnimationComponent&) = delete;
        AnimationComponent& operator= (const AnimationComponent&) = delete;

        virtual void initialize() override{}

        void setSkeleton(const Ra::Core::Animation::Skeleton& skel);

        inline Ra::Core::Animation::Skeleton& getSkeleton() { return m_skel; }
        Ra::Core::Animation::WeightMatrix getWeights() const;
        Ra::Core::Animation::Pose getRefPose() const;


        /// Update the skeleton with an animation.
        void update(Scalar dt);
        void reset();
        void setXray(bool on) const;

        void toggleSkeleton( const bool status );
        void toggleAnimationTimeStep( const bool status );
        void setSpeed( const Scalar value );
        void toggleSlowMotion( const bool status );
        void setAnimation( const uint i );

        uint getBoneIdx(Ra::Core::Index index) const ;
        Scalar getTime() const;


        void handleSkeletonLoading(const Ra::Asset::HandleData* data,
                                   const std::vector<Ra::Core::Index> &duplicateTable,
                                   uint nbMeshVertices );
        void handleAnimationLoading( const std::vector< Ra::Asset::AnimationData* > data );

        //
        // Editable interface
        //

        virtual bool canEdit(Ra::Core::Index roIdx) const override;

        virtual Ra::Core::Transform getTransform(Ra::Core::Index roIdx) const override;

        virtual void setTransform(Ra::Core::Index roIdx, const Ra::Core::Transform& transform) override;

    public:
        // debug function to display the hierarchy
        void printSkeleton(const Ra::Core::Animation::Skeleton& skeleton);

        //
        // Loading data functions
        //

        void setWeights (Ra::Core::Animation::WeightMatrix m);


        // Component communication
        void setContentName (const std::string name);
        void setupIO( const std::string& id );

        const Ra::Core::Animation::Skeleton*     getSkeletonOutput() const;
        const Ra::Core::Animation::RefPose*      getRefPoseOutput() const;
        const Ra::Core::Animation::WeightMatrix* getWeightsOutput() const;
        const bool*                              getWasReset() const;
        const Ra::Core::Animation::Animation*    getAnimation() const;
        const Scalar* getTimeOutput() const;

    private:
        // Internal function to create the skinning weights.
        void createWeightMatrix( const Ra::Asset::HandleData* data,
                                 const std::map< uint, uint >& indexTable,
                                 const std::vector<Ra::Core::Index>& duplicateTable,
                                 uint nbMeshVertices );

        // Internal function to create the bone display objects.
        void setupSkeletonDisplay();


    private:
        std::string m_contentName;

        Ra::Core::Animation::Skeleton m_skel; // Skeleton
        Ra::Core::Animation::RefPose m_refPose; // Ref pose in model space.
        std::vector<Ra::Core::Animation::Animation> m_animations;
        Ra::Core::Animation::WeightMatrix m_weights; // Skinning weights ( should go in skinning )
        std::vector< std::unique_ptr<SkeletonBoneRenderObject> > m_boneDrawables ; // Vector of bone display objects
        uint   m_animationID;
        bool   m_animationTimeStep;
        Scalar m_animationTime;
        std::vector< Scalar > m_dt;
        Scalar m_speed;
        bool   m_slowMo;

        bool m_wasReset;
        bool m_resetDone;
    };

}

#endif //ANIMPLUGIN_ANIMATION_COMPONENT_HPP_
