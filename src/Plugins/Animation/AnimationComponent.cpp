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

            renderObjects.push_back( boneRenderObject->idx );
        }
    }

    bool AnimationComponent::picked(uint drawableIdx) const
    {
        uint i = 0;
        for (auto dr: m_boneDrawables)
        {
            if ( dr->idx == static_cast<int>( drawableIdx ) )
            {
                m_selectedBone = i;
                return true;
            }
            ++i;
        }
        return false;
    }

    void AnimationComponent::getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const
    {
        if ( m_selectedBone < 0 || m_selectedBone >= m_skel.size() )
        {
            m_selectedBone = 0;
        }

        CORE_ASSERT(m_selectedBone >= 0 && m_selectedBone < m_skel.size(), "Oops");
        //for (uint i = 0; i < m_skel.size(); ++i)
        uint i = m_selectedBone;
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

        Ra::Core::Transform tr = m_skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL)[boneIdx];
        // TODO (val) this code is copied from entity.cpp and could be factored.
        for(const auto& entry: prop.primitives)
        {
            const Ra::Engine::EditablePrimitive& prim = entry.primitive;
            switch (prim.getType())
            {
                case Ra::Engine::EditablePrimitive::POSITION:
                {
                    CORE_ASSERT(prim.getName() == "Position", "Inconsistent primitive");

                    // Val : ignore translation for now  (todo : use the flags in primitive).
                    // tr.translation() = prim.asPosition();
                }
                break;

                case Ra::Engine::EditablePrimitive::ROTATION:
                {
                    CORE_ASSERT(prim.getName() == "Rotation", "Inconsistent primitive");
                    tr.linear() = prim.asRotation().toRotationMatrix();
                }
                break;

                default:
                {
                    CORE_ASSERT(false, "Wrong primitive type in property");
                }
                break;
            }
        }

        // Transforms are edited in model space but applied to local space.
        const Ra::Core::Transform& TBoneModel = m_skel.getTransform(boneIdx, Ra::Core::Animation::Handle::SpaceType::MODEL);
        const Ra::Core::Transform& TBoneLocal= m_skel.getTransform(boneIdx, Ra::Core::Animation::Handle::SpaceType::LOCAL);
        auto diff = TBoneModel.inverse() *  tr;

        m_skel.setTransform(boneIdx,TBoneLocal * diff,  Ra::Core::Animation::Handle::SpaceType::LOCAL);
    }

    void AnimationComponent::set(const Ra::Core::Animation::Skeleton& skel)
    {
        m_skel = skel;
        m_refPose = skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL);
    }

    void AnimationComponent::update(Scalar dt)
    {
        if (dt > 0.5) // Ignore large dt that appear when the engine is paused (while loading a file for instance)
            dt = 0;

        // Compute the elapsed time
        m_animationTime += dt;

        // get the current pose from the animation
        if (dt > 0 && m_animations.size() > 0)
        {
            Ra::Core::Animation::Pose currentPose = m_animations[0].getPose(m_animationTime);

            // update the pose of the skeleton
            m_skel.setPose(currentPose, Ra::Core::Animation::Handle::SpaceType::LOCAL);
        }

        // update the render objects
        for (SkeletonBoneRenderObject* bone : m_boneDrawables)
        {
            bone->update();
        }
    }

    void AnimationComponent::handleLoading(const AnimationLoader::AnimationData& data)
    {
        LOG( logDEBUG ) << "Animation component: loading a skeleton";

        Ra::Core::Animation::Skeleton skeleton = Ra::Core::Animation::Skeleton(data.hierarchy.size());
        skeleton.m_graph = data.hierarchy;
        skeleton.setPose(data.pose, Ra::Core::Animation::Handle::SpaceType::LOCAL);
        skeleton.setName( data.name );

        if (data.boneNames.size() == data.hierarchy.size())
        {
            for (int i = 0; i < skeleton.m_graph.size(); i++)
                skeleton.setLabel(i, data.boneNames[i]);
        }
        else  // Auto-naming
        {
            for (int i = 0; i < skeleton.m_graph.size(); i++)
                skeleton.setLabel(i, m_name + std::string("Bone_") + std::to_string(i));
        }

        set(skeleton);

        m_animations = data.animations;
        m_animationTime = 0;
        m_weights = data.weights;

        initialize();
    }

    void AnimationComponent::reset()
    {
        m_animationTime= 0;
        // reset mesh m_meshComponent->
    }

    void AnimationComponent::setMeshComponent(FancyMeshPlugin::FancyMeshComponent* component)
    {
        m_meshComponent = component;
    }

    Ra::Core::Animation::Pose AnimationComponent::getRefPose() const
    {
        return m_refPose;
    }

    FancyMeshPlugin::FancyMeshComponent* AnimationComponent::getMeshComponent() const
    {
        return m_meshComponent;
    }

    Ra::Core::Animation::WeightMatrix AnimationComponent::getWeights() const
    {
        return m_weights;
    }
}
