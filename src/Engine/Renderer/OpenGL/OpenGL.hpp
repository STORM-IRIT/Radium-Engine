#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

/// This file provides portable inclusion of OpenGL headers.
/// In addition it defines the following debug macros
/// GL_ASSERT(x) : executes code x and, if in debug, asserts when an openGL error is detected.
/// GL_CHECK_ERROR : queries the last openGL error in debug (no effect in release)
/// glFlushError() : Ignores the previous openGL errors (no effect in release).

#include <Engine/RaEngine.hpp>
#include <Core/Log/Log.hpp>

#ifdef OS_MACOS // Apple has different gl includes.

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#define __gl_h_

#else // Non-apple OSes.

#ifdef OS_WINDOWS // Windows requires GLEW.
#include <Engine/Renderer/OpenGL/glew.h>
#endif // OS_WINDOWS

// Now the standard OpenGL includes.
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <Engine/Renderer/OpenGL/glext.h>

#endif // OS_MACOS

/// Checks that an openGLContext is available (mostly for debug checks and asserts).
inline bool checkOpenGLContext()
{
    return glGetString( GL_VERSION ) != 0;
}

/// Gets the openGL error string (emulates gluErrorString())
inline const char* glErrorString(GLenum err)
{
    switch ( err )
    {
        case GL_INVALID_ENUM:
            return " Invalid enum : An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n";
        case GL_INVALID_VALUE:
            return " Invalid value : A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n";
        case GL_INVALID_OPERATION:
            return " Invalid operation : The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return " Invalid framebuffer operation : The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n";
        case GL_OUT_OF_MEMORY:
            return " Out of memory : There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n";
        // case GL_STACK_UNDERFLOW:
        //    return " Stack underflow : An attempt has been made to perform an operation that would cause an internal stack to underflow.\n";
        //case GL_STACK_OVERFLOW:
        //    return " Stack overflow : An attempt has been made to perform an operation that would cause an internal stack to overflow.\n";
        case GL_NO_ERROR:
            return " No error\n";
        default:
            return " Unknown GL error\n";
    }
}

// FIXME(Charly): Fix logging
#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const char* errBuf = glErrorString(err); \
            LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                          << ", " << STRINGIFY(x) << ") : " << errBuf << "(" \
                          << err << " : 0x" << std::hex << err << std::dec << ")."; \
            BREAKPOINT(0);\
        } \
    }

/// This macro will query the last openGL error.
#define GL_CHECK_ERROR \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const char* errBuf = glErrorString(err); \
            LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                          << ", glCheckError()) : " << errBuf << "(" \
                          << err << " : 0x" << std::hex << err << std::dec << ")."; \
            BREAKPOINT(0);\
        } \
    }

/// Ignore the previous openGL errors.
#define glFlushError() glGetError()

#else // Release version ignores the checks and errors.
#define GL_ASSERT(x) x
#define GL_CHECK_ERROR {}
#define glFlushError() {}
#endif // _DEBUG

#endif // RADIUMENGINE_OPENGL_HPP
