#ifndef RADIUMENGINE_BINDABLESHADERPROGRAM_HPP
#define RADIUMENGINE_BINDABLESHADERPROGRAM_HPP

#include <Engine/Renderer/RenderQueue/Bindable.hpp>

namespace Ra { namespace Engine { class ShaderProgram;    } }
namespace Ra { namespace Engine { class RenderParameters; } }

namespace Ra { namespace Engine {

class ShaderKey
{
public:
    explicit ShaderKey(ShaderProgram* shader);
    virtual ~ShaderKey();

    virtual void bind() const;
    void bind(const RenderParameters& params) const;

    ShaderProgram* getShader() const { return m_shader; }

    bool operator<(const ShaderKey& other) const;

private:
    ShaderProgram* m_shader;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLESHADERPROGRAM_HPP
