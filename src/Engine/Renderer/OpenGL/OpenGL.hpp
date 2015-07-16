#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

#ifdef OS_WINDOWS
#include <windows.h>

# define __STRING(x) #x
#endif // OS_WINDOWS

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdio>

namespace Ra { namespace Engine {

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
                fprintf(stderr, "OpenGL error (%s:%d, %s) : %s (%d)\n", \
						__FILE__, __LINE__, __STRING(x), errBuf, err); \
		} \
    }

#define GL_CHECK_ERROR \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            fprintf(stderr, "OpenGL error (%s:%d, glCheckError()) : %s (%d).\n",\
                    __FILE__, __LINE__, gluErrorString(err), err);\
		} \
    }

#define glFlushError() glGetError()

#else
#define GL_ASSERT(x) x
#define GL_CHECK_ERROR() 
#define glFlushError() 
#endif

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_OPENGL_HPP
