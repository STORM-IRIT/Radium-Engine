#include "AnimationLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Core/Log/Log.hpp>
#include <vector>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <iostream>

namespace AnimationPlugin
{
	namespace AnimationLoader
	{	
		void assimpToCore(const aiMatrix4x4& inMatrix, Ra::Core::Transform &outMatrix);
		void recursiveSkeletonRead(const aiNode* node, aiMatrix4x4 currentTransform, const std::vector<aiBone*>& bones, 
								   Ra::Core::Graph::AdjacencyList& hierarchy, Ra::Core::Animation::Pose& pose, int parent);
	
		AnimationData loadFile( const std::string& name )
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
				aiMesh* mesh = scene->mMeshes[0];
				std::vector<aiBone*> bones;
				for (int i = 0; i < mesh->mNumBones; i++)
					bones.push_back(mesh->mBones[i]);
				
				// find the bone nodes and create the corresponding skeleton
				recursiveSkeletonRead(scene->mRootNode, scene->mRootNode->mTransformation, bones, animData.hierarchy, animData.pose, -1);
			}
			else
			{
				LOG(logDEBUG) << "Invalid mesh count found: " << scene->mNumMeshes;
			}
			//recursiveSkeletonRead(scene->mRootNode, scene);
				
			return animData;
		}
		
		void recursiveSkeletonRead(const aiNode* node, aiMatrix4x4 currentTransform, const std::vector<aiBone*>& bones, 
								   Ra::Core::Graph::AdjacencyList& hierarchy, Ra::Core::Animation::Pose& pose, int parent)
		{
			currentTransform *= node->mTransformation;
			bool isBoneNode = false;
			for (aiBone* bone : bones)
			{
				if (bone->mName == node->mName)
					isBoneNode = true;
			}
			
			int currentIndex = parent;
			if (isBoneNode)
			{
				// store the bone in the hierarchy
				currentIndex = hierarchy.add(parent);
			
				// store the transform for the bone
				Ra::Core::Transform tr;
				assimpToCore(currentTransform, tr);
				pose.push_back(tr);
				
				// initialize the transform for the child bones
				currentTransform = aiMatrix4x4();
			}
			
			for (int i = 0; i < node->mNumChildren; i++)
				recursiveSkeletonRead(node->mChildren[i], currentTransform, bones, hierarchy, pose, currentIndex);
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
	}
}
