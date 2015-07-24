#include "MainWindow.hpp"

#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra 
{
    inline const std::vector<Core::KeyEvent>& Gui::MainWindow::getKeyEvents() const
    {
        return m_keyEvents;
    }

    inline const std::vector<Core::MouseEvent>& Gui::MainWindow::getMouseEvents() const
    {
        return m_mouseEvents;
    }

    inline void Gui::MainWindow::flushEvents()
    {
        m_mouseEvents.clear();
        m_keyEvents.clear();
    }

    inline void Gui::MainWindow::viewerMousePressEvent(QMouseEvent * event)
    {
        m_mouseEvents.push_back(mouseEventQtToRadium(event));
        m_mouseEvents.back().event = Core::MouseEventType::RA_MOUSE_PRESSED;
    }

    inline void Gui::MainWindow::viewerMouseReleaseEvent(QMouseEvent * event)
    {
        m_mouseEvents.push_back(mouseEventQtToRadium(event));
        m_mouseEvents.back().event = Core::MouseEventType::RA_MOUSE_RELEASED;
    }

    inline void Gui::MainWindow::viewerMouseMoveEvent(QMouseEvent * event)
    {
        m_mouseEvents.push_back(mouseEventQtToRadium(event));
        m_mouseEvents.back().event = Core::MouseEventType::RA_MOUSE_MOVED;
    }

    inline void Gui::MainWindow::viewerWheelEvent(QWheelEvent * event)
    {
        // TODO !
        m_mouseEvents.push_back(wheelEventQtToRadium(event));
        m_mouseEvents.back().event = Core::MouseEventType::RA_MOUSE_WHEEL;
    }

}
