#ifndef RADIUMENGINE_BINDABLE_HPP
#define RADIUMENGINE_BINDABLE_HPP

namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class Bindable
{
public:
    Bindable() = default;
    virtual ~Bindable() = default;

    virtual void bind(ShaderProgram* shader) const = 0;
};

}
}

#endif // RADIUMENGINE_BINDABLE_HPP
