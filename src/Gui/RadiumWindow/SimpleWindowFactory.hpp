#pragma once
#include <Gui/BaseApplication.hpp>
#include <Gui/RaGui.hpp>
#include <Gui/Viewer/Viewer.hpp>
#include <Gui/RadiumWindow/SimpleWindow.hpp>

namespace Ra {
namespace Gui {

/**
 * Factory for a minimal Radium Window.
 * This factory is to be used when instanciating a Ra::Gui::BaseApplication to serve
 * as display controler.
 */
class SimpleWindowFactory : public Ra::Gui::BaseApplication::WindowFactory
{
  public:
    using Ra::Gui::BaseApplication::WindowFactory::WindowFactory;
    bool optionNeedGizmo {true};
    inline Ra::Gui::MainWindowInterface* createMainWindow() const override {
        auto w = new SimpleWindow();
        if ( optionNeedGizmo ) w->getViewer()->createGizmoManager();
        return w;
    }
};

} // namespace Gui
} // namespace Ra
