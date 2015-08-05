#ifndef RADIUMENGINE_BINDABLEMESH_HPP
#define RADIUMENGINE_BINDABLEMESH_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/Bindable/Bindable.hpp>

namespace Ra { namespace Engine { class Mesh; }}

namespace Ra { namespace Engine {

class RA_API BindableMesh : public Bindable
{
public:
    RA_CORE_ALIGNED_NEW
    explicit BindableMesh(Mesh* mesh, uint id);
    virtual ~BindableMesh();

    virtual void bind(ShaderProgram* shader) const override;
    void render() const;

    bool operator<(const BindableMesh& other) const;

private:
    Mesh* m_mesh;
    uint m_id;
    Core::Color m_idAsColor;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLEMESH_HPP
