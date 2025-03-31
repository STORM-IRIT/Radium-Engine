#include <Headless/CLI/App.hpp>
#include <Headless/CLI/Error.hpp>
#include <Headless/CLIBaseApplication.hpp>

namespace Ra {

namespace Headless {
int CLIBaseApplication::init( int argc, const char** argv ) {
    try {
        m_cmdLineParser.parse( argc, argv );
    }
    catch ( const CLI::ParseError& e ) {
        return m_cmdLineParser.exit( e ) + 1;
    }
    return 0;
}

} // namespace Headless
} // namespace Ra
