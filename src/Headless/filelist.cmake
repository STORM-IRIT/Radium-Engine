# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Headless"
# from ./scripts directory
# ----------------------------------------------------

set(headless_sources CLIBaseApplication.cpp CLIViewer.cpp OpenGLContext/EglOpenGLContext.cpp
                     OpenGLContext/GlfwOpenGLContext.cpp
)

set(headless_headers
    CLI/App.hpp
    CLI/CLI.hpp
    CLI/Config.hpp
    CLI/ConfigFwd.hpp
    CLI/Error.hpp
    CLI/Formatter.hpp
    CLI/FormatterFwd.hpp
    CLI/Macros.hpp
    CLI/Option.hpp
    CLI/Split.hpp
    CLI/StringTools.hpp
    CLI/Timer.hpp
    CLI/TypeTools.hpp
    CLI/Validators.hpp
    CLI/Version.hpp
    CLIBaseApplication.hpp
    CLIViewer.hpp
    OpenGLContext/EglOpenGLContext.hpp
    OpenGLContext/GlfwOpenGLContext.hpp
    OpenGLContext/OpenGLContext.hpp
    RaHeadless.hpp
)

set(headless_inlines CLIBaseApplication.inl CLIViewer.inl)
