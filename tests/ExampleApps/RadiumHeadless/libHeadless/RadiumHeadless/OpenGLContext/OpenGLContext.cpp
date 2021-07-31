#include "OpenGLContext.hpp"

#include <GLFW/glfw3.h>

#include <glbinding/AbstractFunction.h>
#include <glbinding/Binding.h>
#include <glbinding/CallbackMask.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/Version.h>
#include <glbinding/glbinding.h>

#include <glbinding/gl/gl.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/Meta.h>
#include <glbinding-aux/ValidVersions.h>
#include <glbinding-aux/types_to_string.h>

#include <globjects/globjects.h>

#include <iostream>

using namespace gl;
using namespace glbinding;

static void error( int errnum, const char* errmsg ) {
    globjects::critical() << errnum << ": " << errmsg << std::endl;
}

OpenGLContext::OpenGLContext( const std::array<int, 2>& size ) {
    // initialize openGL
    if ( glfwInit() )
    {
        glfwSetErrorCallback( error );
        glfwDefaultWindowHints();
        glfwWindowHint( GLFW_VISIBLE, false );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, true );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        m_offscreenContext =
            glfwCreateWindow( size[0], size[1], "Radium CommandLine Context", nullptr, nullptr );
    }
    if ( m_offscreenContext == nullptr )
    {
        globjects::critical() << "Context creation failed. Terminate execution.";
        glfwTerminate();
    }
    else
    {
        // Initialize globjects (internally initializes glbinding, and registers the current
        // context)
        glfwMakeContextCurrent( m_offscreenContext );
        globjects::init( []( const char* name ) { return glfwGetProcAddress( name ); } );
    }
}
OpenGLContext::~OpenGLContext() {
    glfwTerminate();
}
void OpenGLContext::makeCurrent() const {
    if ( m_offscreenContext ) { glfwMakeContextCurrent( m_offscreenContext ); }
}

void OpenGLContext::doneCurrent() const {
    if ( m_offscreenContext ) { glfwMakeContextCurrent( nullptr ); }
}

bool OpenGLContext::isValid() const {
    return m_offscreenContext != nullptr;
}

std::string OpenGLContext::getInfo() const {
    std::stringstream infoText;
    using ContextInfo = glbinding::aux::ContextInfo;
    makeCurrent();
    infoText << "*** OffScreen OpenGL context ***" << std::endl;
    infoText << "Renderer (glbinding) : " << ContextInfo::renderer() << "\n";
    infoText << "Vendor   (glbinding) : " << ContextInfo::vendor() << "\n";
    infoText << "OpenGL   (glbinding) : " << ContextInfo::version().toString() << "\n";
    infoText << "GLSL                 : " << gl::glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n";
    doneCurrent();

    return infoText.str();
}

void OpenGLContext::show() {
    glfwShowWindow( m_offscreenContext );
}

void OpenGLContext::hide() {
    glfwHideWindow( m_offscreenContext );
}

void OpenGLContext::resize( const std::array<int, 2>& size ) {
    glfwSetWindowSize( m_offscreenContext, size[0], size[1] );
}

void OpenGLContext::swapbuffers() {
    glfwSwapBuffers( m_offscreenContext );
    glfwPollEvents();
}

void OpenGLContext::waitForClose() {
    while ( !glfwWindowShouldClose( m_offscreenContext ) )
    {
        glfwPollEvents();
    }
}
