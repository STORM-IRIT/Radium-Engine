#include <GuiBase/BaseApplication.hpp>


/// Allow singleton-like access to the main app à la qApp.
#if defined(mainApp)
#undef mainApp
#endif
#define mainApp (static_cast<Ra::MainApplication*>(qApp))

namespace Ra
{
    class MainApplication : public Ra::GuiBase::BaseApplication
    {
    public:
        using Ra::GuiBase::BaseApplication::BaseApplication;

    };

}
