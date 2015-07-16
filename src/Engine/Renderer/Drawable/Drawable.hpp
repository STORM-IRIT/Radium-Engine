#ifndef RADIUMENGINE_DRAWABLE_HPP
#define RADIUMENGINE_DRAWABLE_HPP

#include <string>
#include <mutex>

namespace Ra { namespace Engine {

class Drawable
{
public:
    explicit Drawable(const std::string& name) : m_name(name) {}
    ~Drawable() = default;

    virtual const std::string& getName() const final { return m_name; }

    // FIXME (Charly): Is the shader program needed ?
    virtual void draw() = 0;
    virtual void setVisible(bool visible) final;
    virtual bool getVisible() const final;

protected:
    std::string m_name;

    bool m_visible;
    mutable std::mutex m_visibleMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Drawable/Drawable.inl>

#endif // RADIUMENGINE_DRAWABLE_HPP
