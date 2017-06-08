#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/base/File.h>
#include <globjects/NamedString.h>
#include <globjects/base/StringTemplate.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>

#include <cstdio>
#ifdef OS_WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

#include <Core/Log/Log.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {
        // From OpenGL Shading Language 3rd Edition, p215-216
        std::string getShaderInfoLog( GLuint shader )
        {
            int infoLogLen;
            int charsWritten;
            GLchar* infoLog;
            std::stringstream ss;

            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLen );

            if ( infoLogLen > 0 )
            {
                infoLog = new GLchar[infoLogLen];
                // error check for fail to allocate memory omitted
                glGetShaderInfoLog( shader, infoLogLen, &charsWritten, infoLog );
                ss << "InfoLog : " << std::endl << infoLog << std::endl;
                delete[] infoLog;
            }

            return ss.str();
        }

        std::string getProgramInfoLog(GLuint program)
        {
            GLboolean status;

            glGetProgramiv( program, GL_LINK_STATUS, & status );
            if ( status != GL_TRUE )
            {
                int infoLogLen;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);

                if (infoLogLen > 0)
                {
                    int charsWritten;
                    GLchar* infoLog = new GLchar[infoLogLen];
                    glGetProgramInfoLog(program, infoLogLen, &charsWritten, infoLog);
                    std::stringstream ss;
                    ss << "Shader link status : " << std::endl << infoLog << std::endl;
                    delete[] infoLog;
                    return ss.str();
                }
            }
            return "";
        }

        ShaderObject::ShaderObject()
            : m_attached(false)
        {
            m_shader.reset( nullptr );
        }

        ShaderObject::~ShaderObject()
        {

        }

        bool ShaderObject::loadAndCompile( GLenum type, const std::string& filename )
        {
            std::unique_ptr<globjects::AbstractStringSource> shaderSource = globjects::Shader::sourceFromFile( filename );
            std::unique_ptr<globjects::AbstractStringSource> shaderTemplate = globjects::Shader::applyGlobalReplacements( shaderSource.get() );

            globjects::NamedString::create( "Structs.glsl", new globjects::File( "/export/home/ingres/locussol/Radium-Engine/Radium-Engine/Shaders/Structs.glsl" ) );

            m_shader.reset( new globjects::Shader( type, shaderTemplate.get() ) );

            m_shader->setName( filename );

            m_shader->compile();

            return m_shader->checkCompileStatus();
        }

        bool ShaderObject::reloadAndCompile()
        {
            bool success;

            LOG( logINFO ) << "Reloading shader " << m_shader->name();

            success = loadAndCompile( m_shader->type(), m_shader->name() );

            if(!success)
            {
                LOG( logINFO ) << "Failed to reload shader" << m_shader->name();
            }

            return success;
        }

        uint ShaderObject::getId() const
        {
            return m_shader->id();
        }

        globjects::Shader * ShaderObject::getShaderObject()
        {
            return m_shader.get();
        }

        ShaderProgram:: ShaderProgram()
            : m_linked(false)
            , m_shaderId( 0 )
        {
            for ( uint i = 0; i < m_shaderObjects.size(); ++i )
            {
                m_shaderObjects[i] = nullptr;
                m_shaderStatus[i]  = false;
            }
        }

        ShaderProgram::ShaderProgram( const ShaderConfiguration& config )
            : ShaderProgram()
        {
            load( config );
        }

        ShaderProgram::~ShaderProgram()
        {
            if ( m_shaderId != 0 )
            {
                for ( auto shader : m_shaderObjects )
                {
                    if ( shader )
                    {
                        if ((shader->getId() != 0) && shader->m_attached)
                        {
                            GL_ASSERT( glDetachShader( m_shaderId, shader->getId() ) );
                        }
                        delete shader;
                    }
                }
                glDeleteProgram( m_shaderId );
            }
        }

        uint ShaderProgram::getId() const
        {
            return m_shaderId;
        }
        bool ShaderProgram::isOk() const
        {
            bool ok = true;
            for ( uint i = 0; i < ShaderType_COUNT; ++i )
            {
                if ( m_configuration.m_shaders[i] != "" )
                {
                    ok = ( ok && m_shaderStatus[i] );
                }
            }
            return ok;
        }

        void ShaderProgram::loadShader(ShaderType type, const std::string& name, const std::set<std::string>& props)
        {
    #ifdef OS_MACOS
            if (type == ShaderType_COMPUTE)
            {
                LOG(logERROR) << "No compute shader on OsX <= El Capitan";
                return;
            }
    #endif
            ShaderObject* shader = new ShaderObject;
            bool status = shader->loadAndCompile(getTypeAsGLEnum(type), name);
            m_shaderObjects[type] = shader;
            m_shaderStatus[type]  = status;
        }

        GLenum ShaderProgram::getTypeAsGLEnum(ShaderType type) const
        {
            switch(type)
            {
                case ShaderType_VERTEX: return GL_VERTEX_SHADER;
                case ShaderType_FRAGMENT: return GL_FRAGMENT_SHADER;
                case ShaderType_GEOMETRY: return GL_GEOMETRY_SHADER;
                case ShaderType_TESS_EVALUATION: return GL_TESS_EVALUATION_SHADER;
                case ShaderType_TESS_CONTROL: return GL_TESS_CONTROL_SHADER;
#ifndef OS_MACOS
                // FIXED (Mathias) : GL_COMPUTE_SHADER requires OpenGL >= 4.2, Apple provides OpenGL 4.1
                case ShaderType_COMPUTE: return GL_COMPUTE_SHADER;
#endif
                default: CORE_ERROR("Wrong ShaderType");
            }
            // Should never get there
            return GL_ZERO;
        }

        void ShaderProgram::load( const ShaderConfiguration& shaderConfig )
        {
            //    LOG(INFO) << "Loading shader " << shaderConfig.getName() << " <type = " << std::hex << shaderConfig.getType() << std::dec << ">";
            m_configuration = shaderConfig;

            CORE_ERROR_IF(m_configuration.isComplete(), ("Shader program " + shaderConfig.m_name + " misses vertex or fragment shader.").c_str());

            GL_ASSERT( m_shaderId = glCreateProgram() );

            for (size_t i = 0; i < ShaderType_COUNT; ++i)
            {
                if (m_configuration.m_shaders[i] != "")
                {
                    loadShader(ShaderType(i), m_configuration.m_shaders[i], m_configuration.getProperties());
                }
            }

            link();
        }

        void ShaderProgram::link()
        {
            if ( !isOk() )
            {
                return;
            }

            for ( int i=0; i < ShaderType_COUNT; ++i )
            {
                if ( m_shaderObjects[i] )
                {
                    GL_ASSERT( glAttachShader( m_shaderId, m_shaderObjects[i]->getId() ) );
                    m_shaderObjects[i]->m_attached = true;
                }
            }

            GL_ASSERT( glProgramParameteri(m_shaderId, GL_PROGRAM_SEPARABLE, GL_TRUE) );
            GL_ASSERT( glLinkProgram( m_shaderId ) );

            auto log = getProgramInfoLog(m_shaderId);

            if ( log.size() > 0 )
            {
                LOG(logINFO) << "Shader name : " << m_configuration.m_name;
                LOG(logINFO) << log;
            }
        }

        void ShaderProgram::bind() const
        {
            CORE_ASSERT( m_shaderId != 0, "Shader is not initialized" );
            CORE_ASSERT(checkOpenGLContext(), "Meh");
            GL_ASSERT( glUseProgram( m_shaderId ) );
        }

        void ShaderProgram::unbind() const
        {
            GL_ASSERT( glUseProgram( 0 ) );
        }

        void ShaderProgram::reload()
        {
            for ( unsigned int i = 0; i < m_shaderObjects.size(); ++i )
            {
                if ( m_shaderObjects[i] != nullptr )
                {
                    GL_ASSERT( glDetachShader( m_shaderId, m_shaderObjects[i]->getId() ) );
                    m_shaderObjects[i]->reloadAndCompile();
                }
            }

            link();
        }

        ShaderConfiguration ShaderProgram::getBasicConfiguration() const
        {
            ShaderConfiguration basicConfig;
            basicConfig.m_shaders = m_configuration.m_shaders;
            basicConfig.m_name = m_configuration.m_name;
            return basicConfig;
        }

        void ShaderProgram::setUniform( const char* name, int value ) const
        {
            GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, unsigned int value ) const
        {
            GL_ASSERT( glUniform1ui( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, float value ) const
        {
            GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, double value ) const
        {
            float v = static_cast<float>(value);
            GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2f& value ) const
        {
            GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2d& value ) const
        {
            Core::Vector2f v = value.cast<float>();
            GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3f& value ) const
        {
            GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3d& value ) const
        {
            Core::Vector3f v = value.cast<float>();
            GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4f& value ) const
        {
            GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4d& value ) const
        {
            Core::Vector4f v = value.cast<float>();
            GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2f& value ) const
        {
            GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2d& value ) const
        {
            Core::Matrix2f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3f& value ) const
        {
            GL_ASSERT( glUniformMatrix3fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3d& value ) const
        {
            Core::Matrix3f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix3fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4f& value ) const
        {
            GL_ASSERT( glUniformMatrix4fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4d& value ) const
        {
            Core::Matrix4f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix4fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        // TODO : Provide Texture support
        void ShaderProgram::setUniform( const char* name, Texture* tex, int texUnit ) const
        {
            tex->bind( texUnit );
            GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), texUnit ) );
        }


    }
} // namespace Ra
