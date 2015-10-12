#ifndef ANIMATIONLOADER_HPP
#define ANIMATIONLOADER_HPP

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <vector>
#include <string>

namespace AnimationPlugin
{
	namespace AnimationLoader
	{
		struct AnimationData
		{
			
		};
	
		ANIM_PLUGIN_API AnimationData loadFile( const std::string& name );
	}
}

#endif // ANIMATIONLOADER_HPP
