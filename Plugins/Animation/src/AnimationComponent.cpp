#include <AnimationComponent.hpp>

#include <queue>
#include <iostream>

#include <assimp/scene.h>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Utils/Graph/AdjacencyListOperation.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/HandleWeightOperation.hpp>
#include <Core/Animation/Handle/SkeletonUtils.hpp>

#include <Engine/Assets/KeyFrame/KeyTransform.hpp>
#include <Engine/Assets/KeyFrame/KeyPose.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Drawing/SkeletonBoneDrawable.hpp>
#include "Core/Mesh/TriangleMesh.hpp"

namespace AnimationPlugin
{

    bool AnimationComponent::picked(uint drawableIdx)
    {
        for (const auto& dr: m_boneDrawables)
        {
            if ( dr->getRenderObjectIndex() == int( drawableIdx ) )
            {
                m_selectedBone = dr->getBoneIndex();
                return true;
            }
        }
        return false;
    }

    void AnimationComponent::getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const
    {
        if ( m_selectedBone < 0 || uint(m_selectedBone) >= m_skel.size() )
        {
            return;
        }

        CORE_ASSERT(m_selectedBone >= 0 && uint(m_selectedBone) < m_skel.size(), "Oops");
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
        const Ra::Core::Transform& TBoneLocal = m_skel.getTransform(boneIdx, Ra::Core::Animation::Handle::SpaceType::LOCAL);
        auto diff = TBoneModel.inverse() *  tr;

        m_skel.setTransform(boneIdx,TBoneLocal * diff,  Ra::Core::Animation::Handle::SpaceType::LOCAL);
    }

    void AnimationComponent::setSkeleton(const Ra::Core::Animation::Skeleton& skel)
    {
        m_skel = skel;
        m_refPose = skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL);
    }

    void AnimationComponent::update(Scalar dt)
    {
        // Ignore large dt that appear when the engine is paused (while loading a file for instance)
        if (dt > 0.5)
        {
            dt = 0;
        }

        // Compute the elapsed time
        m_animationTime += dt;

        // get the current pose from the animation
        if ( dt > 0 && m_animations.size() > 0)
        {
            m_wasReset = false;
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

    void AnimationComponent::setupSkeletonDisplay()
    {

        for( uint i = 0; i < m_skel.size(); ++i ) {
            if( !m_skel.m_graph.isLeaf( i ) ) {
                SkeletonBoneRenderObject* boneRenderObject = new SkeletonBoneRenderObject( m_skel.getLabel( i ), this, i, getRoMgr());
                m_boneDrawables.push_back(boneRenderObject);
                m_renderObjects.push_back( boneRenderObject->getRenderObjectIndex());
            } else {
                LOG( logDEBUG ) << "Bone " << m_skel.getLabel( i ) << " not displayed.";
            }
        }
    }

    void AnimationComponent::printSkeleton(const Ra::Core::Animation::Skeleton& skeleton)
    {
        std::deque<int> queue;
        std::deque<int> levels;

        queue.push_back(0);
        levels.push_back(0);
        while (!queue.empty())
        {
            int i = queue.front();
            queue.pop_front();
            int level = levels.front();
            levels.pop_front();
            std::cout<<i<<" "<<skeleton.getLabel(i)<<"\t";
            for(auto c : skeleton.m_graph.m_child[i] )
            {
                queue.push_back(c);
                levels.push_back(level+1);
            }

            if( levels.front() != level)
            {
                std::cout<<std::endl;
            }
        }
    }

    void AnimationComponent::reset()
    {
        m_animationTime = 0;
        m_skel.setPose(m_refPose, Ra::Core::Animation::Handle::SpaceType::MODEL);
        for (SkeletonBoneRenderObject* bone : m_boneDrawables)
        {
            bone->update();
        }
        m_wasReset = true;
    }

    Ra::Core::Animation::Pose AnimationComponent::getRefPose() const
    {
        return m_refPose;
    }
    Ra::Core::Animation::WeightMatrix AnimationComponent::getWeights() const
    {
        return m_weights;
    }

    void AnimationComponent::handleSkeletonLoading( const Ra::Asset::HandleData* data, const std::map< uint, uint >& duplicateTable ) {
        std::string name( m_name );
        name.append( "_" + data->getName() );

        std::string skelName = name;
        skelName.append( "_SKEL" );

        m_skel.setName( name );

        m_contentName = data->getName();

        std::map< uint, uint > indexTable;
        createSkeleton( data, indexTable );

        createWeightMatrix( data, indexTable, duplicateTable );
        m_refPose = m_skel.getPose( Ra::Core::Animation::Handle::SpaceType::MODEL);

        setupSkeletonDisplay();
        setupIO(m_contentName);
    }



    void AnimationComponent::handleAnimationLoading( const std::vector< Ra::Asset::AnimationData* > data ) {
        CORE_ASSERT( ( m_skel.size() != 0 ), "At least a skeleton should be loaded first.");
        if( data.empty() ) return;
        std::map< uint, uint > table;
        std::set< Ra::Asset::Time > keyTime;
        auto handleAnim = data[0]->getFrames();
        for( uint i = 0; i < m_skel.size(); ++i ) {
            for( uint j = 0; j < handleAnim.size(); ++j ) {
                if( m_skel.getLabel( i ) == handleAnim[j].m_name ) {
                    table[j] = i;
                    auto set = handleAnim[j].m_anim.timeSchedule();
                    keyTime.insert( set.begin(), set.end() );
                }
            }
        }

        Ra::Asset::KeyPose keypose;
        Ra::Core::Animation::Pose pose = m_skel.m_pose;

        m_animations.clear();
        m_animations.push_back( Ra::Core::Animation::Animation() );
        for( const auto& t : keyTime ) {
            for( const auto& it : table ) {
                pose[it.second] = ( m_skel.m_graph.isRoot( it.second ) ) ? m_skel.m_pose[it.second] : handleAnim[it.first].m_anim.at( t );
            }
            m_animations[0].addKeyPose( pose, t );
            keypose.insertKeyFrame( t, pose );
        }

        m_animationTime = 0;

    }



    void AnimationComponent::createSkeleton( const Ra::Asset::HandleData* data, std::map< uint, uint >& indexTable ) {
        const uint size = data->getComponentDataSize();
        auto component = data->getComponentData();

        std::set< uint > root;
        for( uint i = 0; i < size; ++i ) {
            root.insert( i );
        }

        auto edgeList = data->getEdgeData();
        for( const auto& edge : edgeList ) {
            root.erase( edge[1] );
        }

        std::vector< bool > processed( size, false );
        for( const auto& r : root ) {
            addBone( -1, r, component, edgeList, processed, indexTable );
        }
    }


    void AnimationComponent::addBone( const int parent,
                                      const uint dataID,
                                      const Ra::Core::AlignedStdVector< Ra::Asset::HandleComponentData >& data,
                                      const Ra::Core::AlignedStdVector< Ra::Core::Vector2i >& edgeList,
                                      std::vector< bool >& processed,
                                      std::map< uint, uint >& indexTable ) {
        if( !processed[dataID] ) {
            processed[dataID] = true;
            uint index = m_skel.addBone( parent, data.at( dataID ).m_frame, Ra::Core::Animation::Handle::SpaceType::MODEL, data.at( dataID ).m_name );
            indexTable[dataID] = index;
            for( const auto& edge : edgeList ) {
                if( edge[0] == dataID ) {
                    addBone( index, edge[1], data, edgeList, processed, indexTable );
                }
            }
        }
    }

    void AnimationComponent::createWeightMatrix( const Ra::Asset::HandleData* data, const std::map< uint, uint >& indexTable, const std::map< uint, uint >& duplicateTable ) {
        // Bad bad bad hack
        // Fails eventually with a 1 vertex mesh
        uint vertexSize = 0;
        for( const auto& item : duplicateTable ) {
            if( item.second > vertexSize ) {
                vertexSize = ( item.second > vertexSize ) ? item.second : vertexSize;
            }
        }
        vertexSize++;
        m_weights.resize( vertexSize, data->getComponentDataSize() );

        //m_weights.resize( data->getVertexSize(), data->getComponentDataSize() );
        //m_weights.setZero();
        for( const auto& it : indexTable ) {
            const uint idx = it.first;
            const uint col = it.second;
            const uint size = data->getComponent( idx ).m_weight.size();
            for( uint i = 0; i < size; ++i ) {
                const uint   row = duplicateTable.at( data->getComponent( idx ).m_weight[i].first );
                const Scalar w   = data->getComponent( idx ).m_weight[i].second;
                m_weights.coeffRef( row, col ) = w;
            }
        }

        Ra::Core::Animation::checkWeightMatrix( m_weights, false );
    }

    void AnimationComponent::setupIO(const std::string &id)
    {
        Ra::Engine::ComponentMessenger::GetterCallback skelOut = std::bind( &AnimationComponent::getSkeletonOutput, this );
        Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::Animation::Skeleton>( getEntity(), this, id, skelOut);
        Ra::Engine::ComponentMessenger::GetterCallback refpOut = std::bind( &AnimationComponent::getRefPoseOutput, this );
        Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::Animation::Pose>( getEntity(), this, id, refpOut);
        Ra::Engine::ComponentMessenger::GetterCallback wOut = std::bind( &AnimationComponent::getWeightsOutput, this );
        Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::Animation::WeightMatrix>( getEntity(), this, id, wOut);
        Ra::Engine::ComponentMessenger::GetterCallback resetOut = std::bind( &AnimationComponent::getWasReset, this );
        Ra::Engine::ComponentMessenger::getInstance()->registerOutput<bool>( getEntity(), this, id, resetOut);
    }

    const void* AnimationComponent::getSkeletonOutput() const
    {
        return &m_skel;
    }

    const void* AnimationComponent::getWeightsOutput() const
    {
        return &m_weights;
    }

    const void* AnimationComponent::getRefPoseOutput() const
    {
        return &m_refPose;
    }

    const void* AnimationComponent::getWasReset() const
    {
        return &m_wasReset;
    }

    void AnimationComponent::toggleXray(bool on) const
    {
        for (const auto& b : m_boneDrawables)
        {
            b->toggleXray(on);
        }
    }
}
