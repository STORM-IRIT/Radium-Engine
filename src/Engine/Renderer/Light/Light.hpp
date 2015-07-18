#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Core/Math/Vector.hpp>

namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class Light
{
public:
    enum LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

public:
    Light(const LightType& type);
    virtual ~Light();

    inline const Core::Color& getColor() const;
    inline void setColor(const Core::Color& color);

    inline const LightType& getType() const;

    virtual void bind(ShaderProgram* shader);

private:
    LightType m_type;

    // FIXME(Charly): Add color intensity
    Core::Color m_color;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/Light.inl>

#endif // RADIUMENGINE_LIGHT_HPP
