#pragma once
#include <Gui/Viewer/WindowQt.hpp>

namespace Ra {
namespace Gui {

class WindowQt::ScopedGLContext
{
  public:
    explicit ScopedGLContext( WindowQt* window ) : m_window( window ) { window->makeCurrent(); }
    ~ScopedGLContext() { m_window->doneCurrent(); }
    ScopedGLContext( const ScopedGLContext& )            = delete;
    ScopedGLContext& operator=( ScopedGLContext const& ) = delete;

  private:
    WindowQt* m_window;
};

inline WindowQt::ScopedGLContext WindowQt::activateScopedContext() {
    return ScopedGLContext { this };
}

} // namespace Gui
} // namespace Ra
