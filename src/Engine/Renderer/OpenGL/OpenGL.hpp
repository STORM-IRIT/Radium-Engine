#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>

#ifdef OS_WINDOWS
#include <GL/glew.h>
# define __STRING(x) #x
#endif // OS_WINDOWS

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <Engine/Renderer/OpenGL/glext.h>

/// This file provide convenience macros for OpenGL.


/// Checks that an openGLContext is available (mostly for debug checks and asserts).
inline bool checkOpenGLContext() { return glGetString(GL_VERSION) != 0; }

// FIXME(Charly): Fix logging
#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const GLubyte* errBuf = gluErrorString(err); \
            if (errBuf == nullptr) \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << __STRING(x) << ") : ERROR_UNKNOWN_BY_GLU (" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            else \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << __STRING(x) << ") : " << errBuf << "(" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            BREAKPOINT(0);\
        } \
    }

/// This macro will query the last openGL error.
#define GL_CHECK_ERROR \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                          << ", glCheckError()) : " << gluErrorString(err) << "(" \
                          << err << " : 0x" << std::hex << err << std::dec << ")."; \
            BREAKPOINT(0);\
        } \
    }

/// Ignore the previous openGL errors.
#define glFlushError() glGetError()

#else
#define GL_ASSERT(x) x
#define GL_CHECK_ERROR {}
#define glFlushError()
#endif

#endif // RADIUMENGINE_OPENGL_HPP
