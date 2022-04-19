#pragma once
#include <Gui/Viewer/WindowQt.hpp>

namespace Ra {
namespace Gui {

class WindowQt::ScopedContext
{
  public:
    explicit ScopedContext( WindowQt* window ) : m_window( window ) { window->makeCurrent(); }
    ~ScopedContext() { m_window->doneCurrent(); }
    ScopedContext( const ScopedContext& ) = delete;
    ScopedContext& operator=( ScopedContext const& ) = delete;

  private:
    WindowQt* m_window;
};

inline WindowQt::ScopedContext WindowQt::activateScopedContext() {
    return ScopedContext { this };
}

} // namespace Gui
} // namespace Ra
