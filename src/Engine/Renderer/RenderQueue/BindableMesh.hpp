#ifndef RADIUMENGINE_BINDABLEMESH_HPP
#define RADIUMENGINE_BINDABLEMESH_HPP

#include <Engine/Renderer/RenderQueue/BindableMesh.hpp>

namespace Ra { namespace Engine {

class BindableMesh : public Bindable
{
public:
    explicit BindableMesh(Mesh* mesh);
    virtual ~BindableMesh();
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLEMESH_HPP
