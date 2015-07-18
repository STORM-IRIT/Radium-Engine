#ifndef RADIUMENGINE_POINTLIGHT_HPP
#define RADIUMENGINE_POINTLIGHT_HPP

#include <Engine/Renderer/Light/Light.hpp>

namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    virtual ~DirectionalLight();

    virtual void bind(ShaderProgram* shader);

    inline void setDirection(const Core::Vector3& pos);
    inline const Core::Vector3& getDirection() const;

private:
    Core::Vector3 m_direction;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
