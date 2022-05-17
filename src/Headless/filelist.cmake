# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run form ./scripts directory
# ./generateFilelistForModule.sh Headless
# ----------------------------------------------------

set(headless_sources CLIBaseApplication.cpp CLIViewer.cpp OpenGLContext/OpenGLContext.cpp)

set(headless_headers
    CLI/App.hpp
    CLIBaseApplication.hpp
    CLI/CLI.hpp
    CLI/ConfigFwd.hpp
    CLI/Config.hpp
    CLI/Error.hpp
    CLI/FormatterFwd.hpp
    CLI/Formatter.hpp
    CLI/Macros.hpp
    CLI/Option.hpp
    CLI/Split.hpp
    CLI/StringTools.hpp
    CLI/Timer.hpp
    CLI/TypeTools.hpp
    CLI/Validators.hpp
    CLI/Version.hpp
    CLIViewer.hpp
    OpenGLContext/OpenGLContext.hpp
    RaHeadless.hpp
)

set(headless_inlines CLIBaseApplication.inl CLIViewer.inl)
