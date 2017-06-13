#define GLM_ENABLE_EXPERIMENTAL

#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/base/File.h>
#include <globjects/NamedString.h>

#include <glm/gtx/string_cast.hpp>

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

#include <Core/Math/GlmAdapters.inl>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {

        ShaderObject::ShaderObject()
        {
        }

        ShaderObject::~ShaderObject()
        {
        }

        bool ShaderObject::loadAndCompile( GLenum type, const std::string& filename )
        {
            std::unique_ptr<globjects::AbstractStringSource> shaderSource = globjects::Shader::sourceFromFile( filename );
            std::unique_ptr<globjects::AbstractStringSource> shaderTemplate = globjects::Shader::applyGlobalReplacements( shaderSource.get() );

            // Paths in which globjects will be looking for shaders includes.
            // Shaders are all situated in Shaders/ folder so the only path needed for shaders is the current directory.
            globjects::Shader::IncludePaths includePaths;
            includePaths.push_back( "/." );

            m_shader.reset( new globjects::Shader( type, shaderTemplate.get(), includePaths ) );
            m_shader->setName( filename );

            // Create, if they weren't already created, named strings which correspond to shader files that you want to
            // use in shaders's includes.
            if( !globjects::NamedString::isNamedString("/Helpers.glsl") )
            {
                new globjects::NamedString( "/Helpers.glsl", new globjects::File( "Shaders/Helpers.glsl" ) );
            }

            if( !globjects::NamedString::isNamedString("/Structs.glsl") ) {
                new globjects::NamedString( "/Structs.glsl", new globjects::File( "Shaders/Structs.glsl" ) );
            }

            if( !globjects::NamedString::isNamedString("/LightingFunctions.glsl") ) {
                new globjects::NamedString( "/LightingFunctions.glsl", new globjects::File( "Shaders/LightingFunctions.glsl" ) );
            }

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

        ShaderProgram::ShaderProgram()
            : m_linked( false )
            , m_shaderId( 0 )
            , m_program( nullptr )
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
                        if ( shader->getId() != 0 )
                        {
                            m_program->detach( shader->getShaderObject() );
                        }
                        delete shader;
                    }
                }
                glDeleteProgram( m_shaderId );
            }
        }

        uint ShaderProgram::getId() const
        {
            return m_program->id();
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

            m_program = globjects::Program::create();
            m_shaderId = m_program->id();
            m_program->setName( shaderConfig.m_name );

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
                    m_program->attach( m_shaderObjects[i]->getShaderObject() );
                }
            }

            m_program->setParameter( GL_PROGRAM_SEPARABLE, GL_TRUE );

            m_program->link();
        }

        void ShaderProgram::bind() const
        {
            CORE_ASSERT( m_shaderId != 0, "Shader is not initialized" );
            CORE_ASSERT(checkOpenGLContext(), "Meh");
            m_program->use();
        }

        void ShaderProgram::unbind() const
        {
            m_program->release();
        }

        void ShaderProgram::reload()
        {
            for ( unsigned int i = 0; i < m_shaderObjects.size(); ++i )
            {
                if ( m_shaderObjects[i] != nullptr )
                {
                    m_program->detach( m_shaderObjects[i]->getShaderObject() );
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
            m_program->setUniform( name, value );
        }

        void ShaderProgram::setUniform( const char* name, unsigned int value ) const
        {
            m_program->setUniform(name, value);
        }

        void ShaderProgram::setUniform( const char* name, float value ) const
        {
            m_program->setUniform( name, value );
        }

        void ShaderProgram::setUniform( const char* name, double value ) const
        {
            float v = static_cast<float>(value);

            m_program->setUniform( name, v );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2d& value ) const
        {
            Core::Vector2f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3d& value ) const
        {
            Core::Vector3f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4d& value ) const
        {
            Core::Vector4f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2d& value ) const
        {
            Core::Matrix2f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3d& value ) const
        {
            Core::Matrix3f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4d& value ) const
        {
            Core::Matrix4f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        // TODO : Provide Texture support
        void ShaderProgram::setUniform( const char* name, Texture* tex, int texUnit ) const
        {
            tex->bind( texUnit );

            m_program->setUniform( name, texUnit );
        }


    }
} // namespace Ra
