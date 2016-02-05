#include "AnimationLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Core/Log/Log.hpp>
#include <vector>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>
#include <iostream>
#include <map>
#include <string.h>
#include <set>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <Core/Mesh/MeshTypes.hpp>

#include <Core/Animation/Handle/HandleWeightOperation.hpp>

namespace AnimationPlugin
{
    namespace AnimationLoader
    {
        struct aiStringComparator
        {
            bool operator()(const aiString& left, const aiString& right) const
            {
                return strcmp(left.C_Str(), right.C_Str()) < 0;
            }
        };
        typedef std::map<aiString, int, aiStringComparator> BoneMap;

        void recursiveSkeletonRead(const aiNode* node, aiMatrix4x4 accTransform, BoneMap& bones, AnimationData& data, int parent);
        void assimpToCore(const aiMatrix4x4& inMatrix, Ra::Core::Transform &outMatrix);
        void assimpToCore(const aiVector3D& inTranslation, const aiQuaternion& inRotation, const aiVector3D& inScaling, Ra::Core::Transform& outTransform);
        void assimpToCore(const aiQuaternion& inQuat, Ra::Core::Quaternion& outQuat);
        void assimpToCore(const aiVector3D& inVec, Ra::Core::Vector3& outVec);
        void getUniqueKeyTimes(aiAnimation* animation, std::vector<double> &times);
        void getTransformFromKey(const aiNodeAnim* key, int i, Ra::Core::Transform& keyTransform);
        void checkWeights( AnimationData& data );

        void recursiveSkeletonRead(const aiNode* node, aiMatrix4x4 accTransform, BoneMap &boneMap, AnimationData& data, int parent)
        {
            aiMatrix4x4 currentTransform  = accTransform * node->mTransformation;
            bool isBoneNode = boneMap.find(node->mName) != boneMap.end();
            int currentIndex = parent;

            if(!isBoneNode && node->mNumChildren == 0 && parent != -1) // Catch the end bones
                isBoneNode = true;

            if(isBoneNode)
            {
                LOG( logDEBUG ) << "Skeleton node found.";
                LOG( logDEBUG ) << "Bone name: " << node->mName.C_Str();
                if(parent == -1)
                {
                    assimpToCore(accTransform, data.baseTransform);
                }

                // store the bone in the hierarchy
                currentIndex = data.hierarchy.addNode(parent);
                // store the index in the BoneMap
                boneMap[node->mName] = currentIndex;

                // store the transform forthe bone
                Ra::Core::Transform tr;
                assimpToCore(currentTransform, tr);
                data.pose.push_back(tr);

                // initialize the transform forthe child bones
                currentTransform = aiMatrix4x4();
            }

            for(uint i = 0; i < node->mNumChildren; ++i)
            LOG( logDEBUG ) << "Child :" << node->mChildren[i]->mName.C_Str();
            for(uint i = 0; i < node->mNumChildren; ++i)
                recursiveSkeletonRead(node->mChildren[i], currentTransform, boneMap, data, currentIndex);
        }

        void getTransformFromKey(const aiNodeAnim* key, int i, Ra::Core::Transform& keyTransform)
        {
            aiVector3D   keyPosition = key->mPositionKeys[i].mValue;
            aiQuaternion keyRotation = key->mRotationKeys[i].mValue;
            aiVector3D   keyScaling  = key->mScalingKeys[i].mValue;

            // convert the key to a transform matrix
            assimpToCore(keyPosition, keyRotation, keyScaling, keyTransform);
        }

        void getUniqueKeyTimes(aiAnimation* animation, std::vector<double>& times)
        {
            int channelCount = animation->mNumChannels;
            std::set<double> timeSet;
            for(uint i = 0; i < channelCount; ++i)
            {
                aiNodeAnim* currentNodeAnim = animation->mChannels[i];

                int channelKeyCount = currentNodeAnim->mNumRotationKeys;
                for(uint j = 0; j < channelKeyCount; ++j)
                {
                    const aiVectorKey& positionKey = currentNodeAnim->mPositionKeys[j];
                    const aiQuatKey& rotationKey = currentNodeAnim->mRotationKeys[j];
                    const aiVectorKey& scalingKey = currentNodeAnim->mScalingKeys[j];

                    CORE_ASSERT(positionKey.mTime == rotationKey.mTime && positionKey.mTime == scalingKey.mTime, "Invalid key times");

                    timeSet.insert(positionKey.mTime);
                }
            }
            std::copy(timeSet.begin(), timeSet.end(), std::back_inserter(times));
        }

        void assimpToCore( const aiMatrix4x4& inMatrix, Ra::Core::Transform& outMatrix )
        {
            for( uint i = 0; i < 4; ++i )
            {
                for( uint j = 0; j < 4; ++j )
                {
                    outMatrix(i, j) = inMatrix[i][j];
                }
            }
        }

        void assimpToCore(const aiQuaternion& inQuat, Ra::Core::Quaternion& outQuat)
        {
            outQuat = Ra::Core::Quaternion(inQuat.w, inQuat.x, inQuat.y, inQuat.z);
        }

        void assimpToCore(const aiVector3D& inVec, Ra::Core::Vector3& outVec)
        {
            outVec = Ra::Core::Vector3(inVec.x, inVec.y, inVec.z);
        }

        void assimpToCore(const aiVector3D &inTranslation, const aiQuaternion &inRotation, const aiVector3D &inScaling, Ra::Core::Transform& outTransform)
        {
            Ra::Core::Vector3 translation;
            Ra::Core::Vector3 scaling;
            Ra::Core::Quaternion rotation;
            assimpToCore(inTranslation, translation);
            assimpToCore(inScaling, scaling);
            assimpToCore(inRotation, rotation);
            outTransform.fromPositionOrientationScale(translation, rotation, scaling);
        }

        void checkWeights( AnimationData &data )
        {
            Ra::Core::Graph::AdjacencyList&    graph   = data.hierarchy;
            Ra::Core::Animation::WeightMatrix& weights = data.weights;
            const uint g_size = graph.size();
            const uint w_rows = weights.rows();
            const uint w_cols = weights.cols();

            Ra::Core::Animation::checkWeightMatrix( weights, false );

            if( g_size > w_cols ) {
                LOG( logDEBUG ) << "Adding columns to the matrix...";
                Ra::Core::Animation::WeightMatrix newWeights( w_rows, g_size );
                newWeights.reserve( weights.size() );
                std::map< uint, uint > table;
                for( uint i = 0; i < w_cols; ++i ) {
                    uint j = i;
                    while( graph.isLeaf( j ) ) {
                        ++j;
                    }
                    table[i] = j;
                }
                for( const auto& it : table ) {
                    for( Ra::Core::Animation::WeightMatrix::InnerIterator w_it( weights, it.first ); w_it; ++w_it ) {
                        const uint   i = w_it.row();
                        const Scalar w = w_it.value();
                        newWeights.coeffRef( i, it.second ) = w;
                    }
                }
                weights.swap( newWeights );
                //std::swap( weights, newWeights );
                Ra::Core::Animation::checkWeightMatrix( weights, false );
            }


        }
    }
}
