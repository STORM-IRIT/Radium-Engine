#ifndef ANIMATIONLOADER_HPP
#define ANIMATIONLOADER_HPP

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <vector>
#include <string>
#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <Core/Animation/Pose/Pose.hpp>

namespace AnimationPlugin
{
	namespace AnimationLoader
	{	
		struct AnimationData
		{
			Ra::Core::Graph::AdjacencyList hierarchy;
			Ra::Core::Animation::Pose pose;
		};
	
		ANIM_PLUGIN_API AnimationData loadFile( const std::string& name );
	}
}

#endif // ANIMATIONLOADER_HPP
