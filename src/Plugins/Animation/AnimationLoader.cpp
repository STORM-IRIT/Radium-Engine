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
	
		AnimationData loadFile( const std::string& name, int index)
		{
			Assimp::Importer importer;
	        const aiScene* scene = importer.ReadFile(name, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals |
	                                                  aiProcess_SortByPType | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords);
			
			AnimationData animData;
			
			if (scene == NULL)
			{
				LOG( logERROR ) << "Error while loading file \"" << name << "\" : " << importer.GetErrorString() << ".";
				return animData;
			}
            
			if (scene->mNumMeshes == 1) // only handle 1 mesh per file for now
			{
                // skeleton loading
				aiMesh* mesh = scene->mMeshes[0];
                BoneMap boneMap; // first: name of the boneNode, second: index of the bone in the hierarchy / pose
				animData.weights.resize(mesh->mNumVertices, mesh->mNumBones);// = Ra::Core::Animation::WeightMatrix(mesh->mNumVertices, mesh->mNumBones);
				
				for (int i = 0; i < mesh->mNumBones; i++)
                {
					boneMap[mesh->mBones[i]->mName] = -1; // the true index will get written during the recursive read of the scene
					for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
					{
						aiVertexWeight vertexWeight = mesh->mBones[i]->mWeights[j];
						animData.weights.insert(vertexWeight.mVertexId, i) = vertexWeight.mWeight;
					}
                }
                
				// find the bone nodes and create the corresponding skeleton
				recursiveSkeletonRead(scene->mRootNode, scene->mRootNode->mTransformation, boneMap, animData, -1);
                
                // animation loading
                LOG(logDEBUG) << "Found " << scene->mNumAnimations << " animations ";
                
                if (scene->mNumAnimations > 0)
                {
                    aiAnimation* animation = scene->mAnimations[0];
                    int keyCount = animation->mChannels[0]->mNumRotationKeys; // There SHOULD be 1 key for every bone at each key pose
                    int boneCount = animation->mNumChannels; // This SHOULD be equal to boneMap.size()
                    Scalar animationRate = animation->mTicksPerSecond > 0 ? animation->mTicksPerSecond : 50;

                    for (int i = 0; i < keyCount; i++)
                    {
                        Ra::Core::Animation::Pose currentPose = Ra::Core::Animation::Pose(boneCount);
                        Scalar keyTime = animation->mChannels[0]->mRotationKeys[i].mTime / animationRate;
                        
                        for (int j = 0; j < boneCount; j++)
                        {
                            // retrieve the ith key of the jth joint
                            aiVector3D keyPosition = animation->mChannels[j]->mPositionKeys[i].mValue;
                            aiQuaternion keyRotation = animation->mChannels[j]->mRotationKeys[i].mValue;
                            aiVector3D keyScaling = animation->mChannels[j]->mScalingKeys[i].mValue;
                            
                            // convert the key to a transform matrix
                            Ra::Core::Transform keyTransform;
                            assimpToCore(keyPosition, keyRotation, keyScaling, keyTransform);
                            
                            // insert the key in the ith pose
                            int boneIndex = boneMap[animation->mChannels[j]->mNodeName];
                            currentPose[boneIndex] = keyTransform;
                        }
                        
                        // store the pose in the animation
                        animData.animation.addKeyPose(currentPose, keyTime);
                    }
                    
                    animData.animation.normalize();
                }
                else
                    LOG(logDEBUG) << "No animation was found";
			}
			else
			{
				LOG(logDEBUG) << "Invalid mesh count found: " << scene->mNumMeshes;
			}
				
			return animData;
		}
		
		void recursiveSkeletonRead(const aiNode* node, aiMatrix4x4 accTransform, BoneMap &boneMap, AnimationData& data, int parent)
		{
			aiMatrix4x4 currentTransform  = accTransform * node->mTransformation;
			BoneMap::const_iterator boneIt = boneMap.find(node->mName);
            bool isBoneNode = boneIt != boneMap.end();
			
//			if (!isBoneNode)
//			{
//				bool isParentBoneNode = false;
//				for (aiBone* bone : bones)
//				{
//					if (node->mParent != NULL && bone->mName == node->mParent->mName)
//						isParentBoneNode = true;
//				}
				
//				if (isParentBoneNode) // catch the bone ends
//					isBoneNode = true;
//			}
			
			int currentIndex = parent;
			if (isBoneNode)
			{
				// store the bone in the hierarchy
				currentIndex = data.hierarchy.addNode(parent);
                // store the index in the BoneMap
                boneMap[node->mName] = currentIndex;
			
				// store the transform for the bone
				Ra::Core::Transform tr;
				assimpToCore(currentTransform, tr);
				data.pose.push_back(tr);
				
				// initialize the transform for the child bones
				currentTransform = aiMatrix4x4();
			}
			
			for (int i = 0; i < node->mNumChildren; i++)
				recursiveSkeletonRead(node->mChildren[i], currentTransform, boneMap, data, currentIndex);
		}
		
		void assimpToCore( const aiMatrix4x4& inMatrix, Ra::Core::Transform& outMatrix )
        {
            for ( uint i = 0; i < 4; ++i )
            {
                for ( uint j = 0; j < 4; ++j )
                {
                    outMatrix( i, j ) = inMatrix[i][j];
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
	}
}
