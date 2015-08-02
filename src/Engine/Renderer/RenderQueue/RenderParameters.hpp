#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class RenderParameters
{
public:
    void bind(ShaderProgram* shader) const {}
};

}
}

#endif // RADIUMENGINE_RENDERPARAMETERS_HPP
