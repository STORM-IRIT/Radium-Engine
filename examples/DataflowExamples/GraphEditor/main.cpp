#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "MainWindow.hpp"

int main( int argc, char* argv[] ) {
    Q_INIT_RESOURCE( GraphEditor );

    QApplication app( argc, argv );
    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "Radium NodeGraph Editor" );
    QCoreApplication::setApplicationVersion( QT_VERSION_STR );
    QCommandLineParser parser;
    parser.setApplicationDescription( QCoreApplication::applicationName() );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "file", "The file to open." );
    parser.process( app );

    MainWindow mainWin;
    if ( !parser.positionalArguments().isEmpty() )
        mainWin.loadFile( parser.positionalArguments().first() );
    mainWin.show();
    return app.exec();
}
