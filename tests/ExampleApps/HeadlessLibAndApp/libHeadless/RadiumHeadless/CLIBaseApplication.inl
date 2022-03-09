#include "CLIBaseApplication.hpp"

template <typename... Args>
CLI::Option* CLIBaseApplication::addOption( Args&&... args ) {
    return m_cmdLineParser.add_option( args... );
}

template <typename... Args>
CLI::Option* CLIBaseApplication::addFlag( Args&&... args ) {
    return m_cmdLineParser.add_flag( args... );
}
