#include "AnimationLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace AnimationPlugin
{
	namespace AnimationLoader
	{
		AnimationData loadFile( const std::string& name )
		{
			Assimp::Importer importer;
	        const aiScene* scene = importer.ReadFile( name,
	                                                  aiProcess_Triangulate |
	                                                  aiProcess_JoinIdenticalVertices |
	                                                  aiProcess_GenSmoothNormals |
	                                                  aiProcess_SortByPType |
	                                                  aiProcess_FixInfacingNormals |
	                                                  aiProcess_CalcTangentSpace |
	                                                  aiProcess_GenUVCoords );
			
			AnimationData data;
			
			return data;
		}
	}
}
