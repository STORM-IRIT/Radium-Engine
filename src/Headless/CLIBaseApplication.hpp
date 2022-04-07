#pragma once
#include <Headless/CLI/CLI.hpp>
#include <Headless/RaHeadless.hpp>

namespace Ra {
namespace Headless {
/**
 * Base class for command line interface Radium application
 *
 */
class HEADLESS_API CLIBaseApplication
{

  protected:
    /** Command-line parameters parser.
     * All derived applications will share parameters --help and --file <scene_file>.
     * Derived classes should extend this parser to add their own parameters
     */
    CLI::App m_cmdLineParser;

  public:
    /// Base constructor.
    CLIBaseApplication() = default;
    /// Base destructor
    virtual ~CLIBaseApplication() = default;

    /// adapter allowing to add a command line option on an application the same way than using
    /// CLI11 directly.
    /// \see https://cliutils.github.io/CLI11/book/chapters/options.html
    template <typename... Args>
    CLI::Option* addOption( Args&&... args );

    /// adapter allowing to add a command line flag on an application the same way than using CLI11
    /// directly.
    /// \see https://cliutils.github.io/CLI11/book/chapters/flags.html
    template <typename... Args>
    CLI::Option* addFlag( Args&&... args );

    /**
     * Application initialization method.
     *
     * This method is called by the main function to initialize the application giving the
     * parameters from the command line.
     * @param argc number of parameter
     * @param argv array of string representing the parameters
     * @return 0 if the application is correctly initialized or an application dependant error code
     * if something went wrong.
     */
    virtual int init( int argc, const char* argv[] );

    /**
     * Run the application.
     *
     * @return 0 if the application was correctly ran or an application dependant error code if
     * something went wrong.
     */
    virtual int run( float timeStep = 0 );
};
} // namespace Headless
} // namespace Ra
#include <Headless/CLIBaseApplication.inl>
