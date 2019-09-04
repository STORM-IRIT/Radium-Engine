#include <MainApplication.hpp>

#include <GuiBase/Utils/KeyMappingManager.hpp>
#include <QHash>

#include <Gui/MainWindow.hpp>

class MainWindowFactory : public Ra::GuiBase::BaseApplication::WindowFactory
{
  public:
    using Ra::GuiBase::BaseApplication::WindowFactory::WindowFactory;
    Ra::GuiBase::MainWindowInterface* createMainWindow() const override {
        return new Ra::Gui::MainWindow();
    }
};

int main( int argc, char** argv ) {
    // This is to prevent random ordering of XML attribs when writing XML files
    // see https://doc.qt.io/qt-5/qhash.html#algorithmic-complexity-attacks for explanations
    // todo : find a way to set this only for XML generated files rather than globally
    qSetGlobalQHashSeed( 0 );

    Ra::MainApplication app( argc, argv, MainWindowFactory() );
    app.setContinuousUpdate( false );
    return app.exec();
}
