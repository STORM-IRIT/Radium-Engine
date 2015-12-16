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
            bool operator()(const aiString& left, const aiString& right)
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

        /*
        AnimationData loadFile( const std::string& name, const FancyMeshPlugin::MeshLoadingInfo& info)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(name, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals |
                                                      aiProcess_SortByPType | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords);

            AnimationData animData;
            animData.hasLoaded = false;

            if( scene == nullptr ) {
                LOG( logERROR ) << "Error while loading file \"" << name << "\" : " << importer.GetErrorString() << ".";
                return animData;
            }
            if( info.index < 0 || info.index >= scene->mNumMeshes ) {
                LOG(logDEBUG) << "Invalid mesh index: " << info.index << " requested, but " << scene->mNumMeshes << " meshes have been found";
                return animData;
            }

            // skeleton loading
            aiMesh* mesh = scene->mMeshes[info.index];
            if( mesh->mNumBones == 0 ) {
                LOG(logDEBUG) << "Mesh #" << info.index << ": no skeleton found.";
                return animData;
            } else {
                LOG(logDEBUG) << "Mesh #" << info.index << ": " << mesh->mNumBones << " bones found.";
            }

            std::string skelName = std::string(mesh->mName.C_Str()) + "_skeleton";
            animData.name = skelName;
            int vertexCount = 0;
            for( uint i = 0; i < info.vertexMap.size(); ++i ) {
                if( info.vertexMap[i] >= vertexCount )
                    vertexCount = info.vertexMap[i] + 1;
            }

            BoneMap boneMap; // first: name of the boneNode, second: index of the bone in the hierarchy / pose
            animData.weights.resize(vertexCount, mesh->mNumBones);

            for( uint i = 0; i < mesh->mNumBones; ++i ) {
                boneMap[mesh->mBones[i]->mName] = -1; // the true index will get written during the recursive read of the scene
                for(uint j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
                {
                    aiVertexWeight vertexWeight = mesh->mBones[i]->mWeights[j];
                    int id = info.vertexMap[vertexWeight.mVertexId];
                    animData.weights.coeffRef(id, i) = vertexWeight.mWeight;
                }
            }

            // find the bone nodes and create the corresponding skeleton
            recursiveSkeletonRead(scene->mRootNode, aiMatrix4x4(), boneMap, animData, -1);

            // Store the names of each bone
            animData.boneNames.resize(boneMap.size());
            for( std::pair<aiString, int> p : boneMap )
            {
                CORE_ASSERT(p.second < boneMap.size(), "Invalid bone index");
                animData.boneNames[p.second] = std::string(p.first.C_Str());
            }
            LOG(logDEBUG) << "Found a skeleton of " << boneMap.size() << " bones";


            checkWeights( animData );

            // animation loading
            LOG(logDEBUG) << "Found " << scene->mNumAnimations << " animations";

            animData.animations.resize(scene->mNumAnimations);
            for(uint k = 0; k < scene->mNumAnimations; k++)
            {
                aiAnimation* animation = scene->mAnimations[k];
                int channelCount = animation->mNumChannels;
                int boneCount = boneMap.size();

                // Get the sorted set of key pose timestamps
                std::vector<double> timeSet;
                getUniqueKeyTimes(animation, timeSet);
                int keyCount = timeSet.size();

                // Allocate the poses
                std::vector<Ra::Core::Animation::Pose> poses;
                for(uint i = 0; i < keyCount; ++i)
                    poses.push_back(Ra::Core::Animation::Pose(boneCount));

                // Track which bones have an animation
                std::vector<bool> animatedBones( boneCount );
                for(uint i = 0; i < boneCount; ++i)
                    animatedBones[i] = false;

                // Add the animated bone transforms to the poses + interpolate when necessary
                for(uint i = 0; i < channelCount; ++i)
                {
                    aiNodeAnim* currentNodeAnim = animation->mChannels[i];
                    if(boneMap.find(currentNodeAnim->mNodeName) == boneMap.end()) // We should be able to ignore bones that do not affect the mesh
                        continue;
                    //CORE_ASSERT(boneMap.find(currentNodeAnim->mNodeName) != boneMap.end(), "Unknown bone channel");

                    int channelKeyCount = currentNodeAnim->mNumPositionKeys;
                    int boneIndex = boneMap[currentNodeAnim->mNodeName];
                    animatedBones[boneIndex] = true;

                    int channelKeyIndex = 0;
                    for(uint j = 0; j < keyCount; ++j)
                    {
                        double channelKeyTime = currentNodeAnim->mPositionKeys[channelKeyIndex].mTime;
                        if(channelKeyTime == timeSet[j])
                        {
                            Ra::Core::Transform keyTransform;
                            getTransformFromKey(currentNodeAnim, channelKeyIndex, keyTransform);
                            poses[j][boneIndex] = keyTransform;
                            if(channelKeyIndex < channelKeyCount - 1)
                                channelKeyIndex++;
                        }
                        else if(channelKeyIndex == 0 || channelKeyIndex == channelKeyCount - 1) // the first channel key is after the current key
                        {
                            Ra::Core::Transform keyTransform;
                            getTransformFromKey(currentNodeAnim, channelKeyIndex, keyTransform);
                            poses[j][boneIndex] = keyTransform;
                        }
                        else if(channelKeyTime > timeSet[j]) // the current key is between two channel keys
                        {
                            // interpolate between the previous and current channel key
                            Ra::Core::Transform previousKeyTransform;
                            Ra::Core::Transform nextKeyTransform;
                            Ra::Core::Transform keyTransform;
                            getTransformFromKey(currentNodeAnim, channelKeyIndex - 1, previousKeyTransform);
                            getTransformFromKey(currentNodeAnim, channelKeyIndex, nextKeyTransform);

                            double prevChannelKeyTime = currentNodeAnim->mPositionKeys[channelKeyIndex - 1].mTime;
                            Scalar t = (timeSet[j] - prevChannelKeyTime) / (channelKeyTime - prevChannelKeyTime);
                            Ra::Core::Animation::interpolateTransforms(previousKeyTransform, nextKeyTransform, t, keyTransform);

                            poses[j][boneIndex] = keyTransform;
                        }
                        else
                        {
                            CORE_ASSERT(false, "AnimationLoader.cpp: should not be there");
                        }

                        if(animData.hierarchy.isRoot(boneIndex))
                            poses[j][boneIndex] = animData.baseTransform * poses[j][boneIndex];
                    }
                }

                // add the non animated bone transforms to the poses
                for(uint i = 0; i < boneCount; ++i)
                {
                    if(!animatedBones[i])
                    {
                        for(uint j = 0; j < keyCount; ++j)
                        {
                            poses[j][i] = animData.pose[i];
                        }
                    }
                }

                // finally create the animation object
                Scalar animationRate = animation->mTicksPerSecond > 0.0 ? animation->mTicksPerSecond : 50.0;
                for(uint i = 0; i < keyCount; ++i)
                {
                    Scalar keyTime = timeSet[i] / animationRate;
                    animData.animations[k].addKeyPose(poses[i], keyTime);
                }
                animData.animations[k].normalize();
            }

            animData.hasLoaded = true;
            return animData;
        }
        */

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

        void checkWeights( AnimationData &data ) {
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
