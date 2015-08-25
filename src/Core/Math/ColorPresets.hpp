#ifndef RADIUMENGINE_COLOR_PRESET_HPP_
#define RADIUMENGINE_COLOR_PRESET_HPP_

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra{
    namespace Core{
        namespace Colors
        {
            Color Black() { return Color(0,0,0,1); }

            Color Red()   { return Color(1,0,0,1); }
            Color Green() { return Color(0,1,0,1); }
            Color Blue()  { return Color(0,0,1,1); }

            Color Yellow()  { return Color(1,1,0,1); }
            Color Magenta() { return Color(1,0,1,1); }
            Color Cyan()    { return Color(0,1,1,1); }

            Color White()   { return Color(1,1,1,1); }
        }
    }
}


#endif //RADIUMENGINE_COLOR_PRESET_HPP_
