#ifndef RADIUMENGINE_BINDABLE_HPP
#define RADIUMENGINE_BINDABLE_HPP

#include <Engine/RaEngine.hpp>

namespace Ra
{
    namespace Engine
    {
        class ShaderProgram;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API Bindable
        {
        public:
            Bindable() {}
            virtual ~Bindable() {}

            virtual void bind( ShaderProgram* shader ) const = 0;
        };

    }
}

#endif // RADIUMENGINE_BINDABLE_HPP
