#ifndef RADIUMENGINE_PARSER_HPP
#define RADIUMENGINE_PARSER_HPP

#include <Engine/RaEngine.hpp>

#include <string>
#include <Engine/Parser/LoadedData.hpp>

namespace Ra
{
    namespace Engine
    {
        namespace Parser
        {
            void parse( const std::string& data, std::vector<LoadedEntity>& loadedData );
        }
    }
}

#endif // RADIUMENGINE_PARSER_HPP