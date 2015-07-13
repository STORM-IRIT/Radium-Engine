#ifndef RADIUMENGINE_DRAWABLE_HPP
#define RADIUMENGINE_DRAWABLE_HPP

#include <string>

namespace Ra
{

class Drawable
{
public:
    explicit Drawable(const std::string& name) : m_name(name) {}
    ~Drawable() = default;

    virtual const std::string& getName() const final { return m_name; }

    // FIXME (Charly): Is the shader program needed ?
    virtual void draw() = 0;

protected:
    std::string m_name;
};

} // namespace Ra

#endif // RADIUMENGINE_DRAWABLE_HPP
