#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

#ifdef OS_WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <windows.h>

# define __STRING(x) #x
#endif // OS_WINDOWS

#include <gl/GL.h>
#include <gl/GLU.h>
#include <QOpenGLFunctions_4_4_Core>
#include <cstdio>

namespace RA
{
#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const GLubyte* errBuf = gluErrorString(err); \
            if (errBuf == nullptr) \
				fprintf(stderr, "OpenGL error (%s:%d, %s) : ERROR_UNKNOWN_BY_GLU (%d)\n", \
						__FILE__, __LINE__, __STRING(x), err); \
			else \
				fprintf(stderr, "OpenGL error (%s:%s, %s) : %s (%d)\n", \
						__FILE__, __LINE__, __STRING(x), errBuf, err); \
		} \
    }

#define GL_CHECK_ERROR \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            std::cerr << "OpenGL error (" << __FILE__ << ":" << __LINE__ << ", glCheckError()) : " \
                      << gluErrorString(err) << "(" << err << ")" << std::endl; \
		} \
    }

#define glFlushError() glGetError()

#else
#define GL_ASSERT(x) x
#define GL_CHECK_ERROR() 
#define glFlushError() 
#endif

} // namespace Ra

#endif // RADIUMENGINE_OPENGL_HPP