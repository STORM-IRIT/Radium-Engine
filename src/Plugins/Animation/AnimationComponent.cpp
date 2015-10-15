#include <Plugins/Animation/AnimationComponent.hpp>

#include <assimp/scene.h>
#include <iostream>
#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>
#include <Core/Utils/Graph/AdjacencyListOperation.hpp>
#include <Core/Animation/Pose/Pose.hpp>

namespace AnimationPlugin
{
    void AnimationComponent::initialize()
    {
        auto edgeList = Ra::Core::Graph::extractEdgeList( m_skel.m_graph );
        for( auto edge : edgeList )
        {
			SkeletonBoneRenderObject* boneRenderObject = new SkeletonBoneRenderObject(m_skel.getName() + " bone " + std::to_string(edge(0)), this, edge, getRoMgr());
            m_boneDrawables.push_back(boneRenderObject);
        }
    }

    void AnimationComponent::getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const
    {
        for (uint i = 0; i < m_skel.size(); ++i)
        {
             const Ra::Core::Transform& tr = m_skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL)[i];
             propsOut.push_back(Ra::Engine::EditableProperty(tr, std::string("Transform ") + std::to_string(i) + "-" + m_skel.getLabel(i)));
        }
    }

    void AnimationComponent::setProperty(const Ra::Engine::EditableProperty &prop)
    {
        int boneIdx = -1;
        CORE_ASSERT(prop.type == Ra::Engine::EditableProperty::TRANSFORM, "Only bones transforms are editable");
        for (uint i =0; i < m_skel.size(); ++i)
        {
            if (prop.name == std::string("Transform ") + std::to_string(i) + "-" + m_skel.getLabel(i))
            {
                boneIdx = i;
                break;
            }
        }
        CORE_ASSERT(boneIdx >=0 , "Property not found in skeleton");
    }

    void AnimationComponent::set(const Ra::Core::Animation::Skeleton& skel)
    {
        m_skel = skel;
        m_refPose = skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL);
    }
    
    void AnimationComponent::update(Scalar dt)
    {
		// Compute the elapsed time
		m_animationTime += dt;
		
        // get the current pose from the animation
        Ra::Core::Animation::Pose currentPose = m_animation.getPose(m_animationTime);
        
        // update the pose of the skeleton
        m_skel.setPose(currentPose, Ra::Core::Animation::Handle::SpaceType::LOCAL);
        
        // update the render objects
        for (SkeletonBoneRenderObject* bone : m_boneDrawables)
        {
			bone->update();
        }
    }

    void AnimationComponent::handleLoading(const AnimationLoader::AnimationData& data)
    {
        LOG( logDEBUG ) << "Animation component: loading a skeleton";
        Ra::Core::Animation::Skeleton skeleton = Ra::Core::Animation::Skeleton();
        skeleton.m_graph = data.hierarchy;
        skeleton.setPose(data.pose, Ra::Core::Animation::Handle::SpaceType::LOCAL); // specify the space type in the AnimationData
        set(skeleton);
        m_animation = data.animation;
        m_animationTime = 0;
        initialize();
    }
}
