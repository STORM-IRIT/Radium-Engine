#ifndef ANIMATIONLOADER_HPP
#define ANIMATIONLOADER_HPP

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <vector>
#include <string>
#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Animation.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace AnimationPlugin
{
	namespace AnimationLoader
	{	
		struct AnimationData
		{
            bool hasLoaded;
			Ra::Core::Graph::AdjacencyList hierarchy;
			Ra::Core::Animation::Pose pose;
            Ra::Core::Animation::Animation animation;
            Ra::Core::Animation::WeightMatrix weights;
		};
	
		ANIM_PLUGIN_API AnimationData loadFile(const std::string& name , int index);
	}
}

#endif // ANIMATIONLOADER_HPP
