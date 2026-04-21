# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Headless"
# from ./scripts directory
# ----------------------------------------------------

set(headless_sources CLIBaseApplication.cpp CLIViewer.cpp OpenGLContext/EglOpenGLContext.cpp
                     OpenGLContext/GlfwOpenGLContext.cpp
)

set(headless_headers
    CLI/CLI11.hpp CLIBaseApplication.hpp CLIViewer.hpp OpenGLContext/EglOpenGLContext.hpp
    OpenGLContext/GlfwOpenGLContext.hpp OpenGLContext/OpenGLContext.hpp RaHeadless.hpp
)
