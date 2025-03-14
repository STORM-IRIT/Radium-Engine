#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <Dataflow/QtGui/GraphEditor/GraphEditorWindow.hpp>

#include <QtNodes/ConnectionStyle>

static void setStyle() {

    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
  {
    "ConnectionStyle": {

      "UseDataDefinedColors": true
    }
  }
  )" );
}

int main( int argc, char* argv[] ) {
    QApplication app( argc, argv );

    setStyle();

    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "Radium NodeGraph Editor" );
    QCoreApplication::setApplicationVersion( QT_VERSION_STR );
    QCommandLineParser parser;
    parser.setApplicationDescription( QCoreApplication::applicationName() );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "file", "The file to open." );
    parser.process( app );

    Ra::Dataflow::QtGui::GraphEditor::GraphEditorWindow mainWin;
    if ( !parser.positionalArguments().isEmpty() )
        mainWin.loadFile( parser.positionalArguments().first() );
    mainWin.show();
    return app.exec();
}
