#ifndef RADIUMENGINE_OPENGL_HPP
#define RADIUMENGINE_OPENGL_HPP

#include <Core/CoreMacros.hpp>

#ifdef OS_WINDOWS
#include <windows.h>
#include <GL/glew.h>
# define __STRING(x) #x
#endif // OS_WINDOWS

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <Core/Log/Log.hpp>

namespace Ra { namespace Engine {

#ifdef _DEBUG
#define GL_ASSERT(x) \
    x; { \
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            const GLubyte* errBuf = gluErrorString(err); \
            if (errBuf == nullptr) \
                LOG(ERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                           << ", " << __STRING(x) << ") : ERROR_UNKNOWN_BY_GLU (" \
                           << err << " : 0x" << std::hex << err << std::dec << ")."; \
			else \
                LOG(ERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                           << ", " << __STRING(x) << ") : " << errBuf << "(" \
                           << err << " : 0x" << std::hex << err << std::dec << ")."; \
		} \
    }

#define GL_CHECK_ERROR \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            LOG(ERROR) << "OpenGL error (" << __FILE__ << ":" << __LINE__ \
                       << ", glCheckError()) : " << gluErrorString(err) << "(" \
                       << err << " : 0x" << std::hex << err << std::dec << ")."; \
	    } \
    }

#define glFlushError() glGetError()

#else
#define GL_ASSERT(x) x
#define GL_CHECK_ERROR {}
#define glFlushError() 
#endif

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_OPENGL_HPP
