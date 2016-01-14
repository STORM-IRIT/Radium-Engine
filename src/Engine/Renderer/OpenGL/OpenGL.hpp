#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Log/Log.hpp>

#ifndef __APPLE__
#ifdef OS_WINDOWS
#include <GL/glew.h>
#endif // OS_WINDOWS

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <Engine/Renderer/OpenGL/glext.h>

/// This file provide convenience macros for OpenGL.


/// Checks that an openGLContext is available (mostly for debug checks and asserts).
inline bool checkOpenGLContext()
{
    return glGetString( GL_VERSION ) != 0;
}

// FIXME(Charly): Fix logging
#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const GLubyte* errBuf = gluErrorString(err); \
            if (errBuf == nullptr) \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << STRINGIFY(x) << ") : ERROR_UNKNOWN_BY_GLU (" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            else \
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

#else

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#define __gl_h_

/// Checks that an openGLContext is available (mostly for debug checks and asserts).
inline bool checkOpenGLContext()
{
    return glGetString( GL_VERSION ) != 0;
}

// FIXME(Charly): Fix logging
#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const char* errBuf = ( (err == GL_INVALID_ENUM) ? " Invalid enum : An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_VALUE) ? " Invalid value : A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_OPERATION) ? " Invalid operation : The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_FRAMEBUFFER_OPERATION) ? " Invalid framebuffer operation : The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_OUT_OF_MEMORY) ? " Out of memory : There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n" : \
 /*               ( (err == GL_STACK_UNDERFLOW) ? " Stack underflow : An attempt has been made to perform an operation that would cause an internal stack to underflow.\n" : \
                ( (err == GL_STACK_OVERFLOW) ? " Stack overflow : An attempt has been made to perform an operation that would cause an internal stack to overflow.\n" : */\
                nullptr) \
/*                )\
                )\
*/                )\
                )\
                )\
                );\
            if (errBuf == nullptr) \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << STRINGIFY(x) << ") : ERROR_UNKNOWN (" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            else \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << STRINGIFY(x) << ") : " << errBuf << "(" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            BREAKPOINT(0);\
        } \
    }

/// This macro will query the last openGL error.
/// TODO : complete with all opengl errors (5 only in the spec)
#define GL_CHECK_ERROR \
    { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const char* errBuf = ( (err == GL_INVALID_ENUM) ? " Invalid enum : An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_VALUE) ? " Invalid value : A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_OPERATION) ? " Invalid operation : The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_INVALID_FRAMEBUFFER_OPERATION) ? " Invalid framebuffer operation : The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n" : \
                ( (err == GL_OUT_OF_MEMORY) ? " Out of memory : There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n" : \
/*               ( (err == GL_STACK_UNDERFLOW) ? " Stack underflow : An attempt has been made to perform an operation that would cause an internal stack to underflow.\n" : \
    ( (err == GL_STACK_OVERFLOW) ? " Stack overflow : An attempt has been made to perform an operation that would cause an internal stack to overflow.\n" : */\
    nullptr)\
/*                )\
              )\
*/                )\
                )\
                )\
                );\
                if (errBuf == nullptr) \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << STRINGIFY(x) << ") : ERROR_UNKNOWN_BY_GLU (" \
                              << err << " : 0x" << std::hex << err << std::dec << ")."; \
            else \
                LOG(logERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                              << ", " << STRINGIFY(x) << ") : " << errBuf << "(" \
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


#endif

#endif // RADIUMENGINE_OPENGL_HPP
