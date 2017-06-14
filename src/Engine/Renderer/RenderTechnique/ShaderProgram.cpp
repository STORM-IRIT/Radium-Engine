#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/NamedString.h>

#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>

#include <fstream>
#include <regex>

#ifdef OS_WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

#include <Core/Math/GlmAdapters.inl>

#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {

        ShaderProgram::ShaderProgram()
            : m_linked( false )
            , m_program( nullptr )
        {
            for ( uint i = 0; i < m_shaderObjects.size(); ++i )
            {
                m_shaderObjects[i] = nullptr;
            }
        }

        ShaderProgram::ShaderProgram( const ShaderConfiguration& config )
            : ShaderProgram()
        {
            load( config );
        }

        ShaderProgram::~ShaderProgram()
        {

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

            std::unique_ptr<globjects::AbstractStringSource> shaderSource = globjects::Shader::sourceFromFile( name );
            std::unique_ptr<globjects::AbstractStringSource> shaderTemplate = globjects::Shader::applyGlobalReplacements( shaderSource.get() );

            // Paths in which globjects will be looking for shaders includes.
            // Shaders are all situated in Shaders/ folder so the only path needed for shaders is the current directory.
            globjects::Shader::IncludePaths includePaths;
            includePaths.push_back( "/." );

            std::unique_ptr<globjects::Shader> shader = globjects::Shader::create( getTypeAsGLEnum( type ), shaderTemplate.get(), includePaths );

            // We apply replacements directly in source to add #version directives and other stuff for example.
            // GLSL files used to include shader's headers are GlobalVertex and GlobalOther, but you can add as many
            // as you wish.
            std::unique_ptr<globjects::File> replacement;

            if( type == ShaderType_VERTEX )
            {
                replacement = globjects::File::create("Shaders/GlobalVertex.glsl");
            }
            else
            {
                replacement = globjects::File::create("Shaders/GlobalOther.glsl");
            }

            // Updating shader's source with file's content.
            std::string oldSource = shader->getSource();
            std::string newSource = replacement->string() + "\n" + oldSource;
            std::unique_ptr<globjects::StaticStringSource> newStringSource = globjects::Shader::sourceFromString( newSource );

            shader->setSource( newStringSource.get() );

            shader->compile();

            m_shaderObjects[type].swap( shader );
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

        ShaderType ShaderProgram::getGLenumAsType(GLenum type) const
        {
            switch(type)
            {
                case GL_VERTEX_SHADER: return ShaderType_VERTEX;
                case GL_FRAGMENT_SHADER: return ShaderType_FRAGMENT;
                case GL_GEOMETRY_SHADER: return ShaderType_GEOMETRY;
                case GL_TESS_EVALUATION_SHADER: return ShaderType_TESS_EVALUATION;
                case GL_TESS_CONTROL_SHADER: return ShaderType_TESS_CONTROL;
#ifndef OS_MACOS
                case GL_COMPUTE_SHADER: return ShaderType_COMPUTE;
#endif
                default: CORE_ERROR("Wrong GLenum");
            }

            // Should never get there
            return ShaderType_COUNT;
        }

        void ShaderProgram::load( const ShaderConfiguration& shaderConfig )
        {
            m_configuration = shaderConfig;

            CORE_ERROR_IF( m_configuration.isComplete(), ("Shader program " + shaderConfig.m_name + " misses vertex or fragment shader.").c_str() );

            m_program = globjects::Program::create();

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
            for ( int i=0; i < ShaderType_COUNT; ++i )
            {
                if ( m_shaderObjects[i] )
                {
                    m_program->attach( m_shaderObjects[i].get() );
                }
            }

            m_program->setParameter( GL_PROGRAM_SEPARABLE, GL_TRUE );

            m_program->link();
        }

        void ShaderProgram::bind() const
        {
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
                    m_program->detach( m_shaderObjects[i].get() );
                    loadShader( getGLenumAsType( m_shaderObjects[i]->type() ), m_shaderObjects[i]->name(), {} );
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

        globjects::Program * ShaderProgram::getProgramObject()
        {
            return m_program.get();
        }
    }
} // namespace Ra
