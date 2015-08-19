#ifndef RADIUMENGINE_LOADEDDATA_HPP
#define RADIUMENGINE_LOADEDDATA_HPP

#include <Core/CoreMacros.hpp>

#include <string>
#include <map>
#include <vector>

#include <Core/Utils/Any.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {
        struct LoadedComponent
        {
            std::string system;
            std::map<std::string, Core::Any> data;
        };

        struct LoadedEntity
        {
            std::string name;
            std::vector<LoadedComponent> data;

            Core::Vector3 position;
            Core::Quaternion orientation;
            Core::Vector3 scale;
        };
    }
}

#endif // RADIUMENGINE_LOADEDDATA_HPP