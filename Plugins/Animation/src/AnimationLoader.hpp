#ifndef ANIMATIONLOADER_HPP
#define ANIMATIONLOADER_HPP

#include "AnimationPlugin.hpp"

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
            std::vector<Ra::Core::Animation::Animation> animations;
            Ra::Core::Animation::WeightMatrix weights;
            Ra::Core::Transform baseTransform;
            std::string name;
            std::vector<std::string> boneNames;
        };

<<<<<<< HEAD:src/Plugins/Animation/AnimationLoader.hpp
//        ANIM_PLUGIN_API AnimationData loadFile(const std::string& name , const FancyMeshPlugin::MeshLoadingInfo &info);
=======
        ANIM_PLUGIN_API AnimationData loadFile(const std::string& name );
>>>>>>> dummyplugin:Plugins/Animation/src/AnimationLoader.hpp
    }
}

#endif // ANIMATIONLOADER_HPP
