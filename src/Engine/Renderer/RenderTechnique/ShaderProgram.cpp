#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdio>
#ifdef OS_WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{

    namespace
    {
        // From OpenGL Shading Language 3rd Edition, p215-216
        std::string getShaderInfoLog( GLuint shader )
        {
            int infoLogLen = 0;
            int charsWritten = 0;
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
    }

    Engine::ShaderObject::ShaderObject()
        : m_id( 0 )
    {
    }

    Engine::ShaderObject::~ShaderObject()
    {
        if ( m_id != 0 )
        {
            GL_ASSERT( glDeleteShader( m_id ) );
        }
    }

    bool Engine::ShaderObject::loadAndCompile( uint type,
                                               const std::string& filename,
                                               const std::set<std::string>& properties )
    {
        m_filename = filename;
        m_type = type;
        m_properties = properties;
        GL_ASSERT( m_id = glCreateShader( type ) );

        std::string shader = load();
        if ( shader != "" )
        {
            compile( shader, properties );
        }

        return check();
    }

    bool Engine::ShaderObject::reloadAndCompile( const std::set<std::string>& properties )
    {
        LOG( logINFO ) << "Reloading shader " << m_filename;
        return loadAndCompile( m_type, m_filename, properties );
    }

    uint Engine::ShaderObject::getId() const
    {
        return m_id;
    }

    std::string Engine::ShaderObject::load()
    {
        std::string shader;

        bool ok = parseFile( m_filename, shader );
        if ( !ok )
        {
            std::ostringstream error;
            error << m_filename << " not found.\n";
            char currentPath[FILENAME_MAX];
            getCurrentDir( currentPath, sizeof( currentPath ) );
            error << "Path : " << currentPath;
            CORE_WARN_IF( !ok, error.str().c_str() );
            shader = "";
            return shader;
        }

        // Keep sure last character is a 0
        if ( shader.back() != '\0' )
        {
            shader.push_back( '\0' );
        }

        return shader;
    }

    void Engine::ShaderObject::compile( const std::string& shader, const std::set<std::string>& properties )
    {
        const char* data[3];
        data[0] = "#version 330\n";

        std::stringstream ss;
        for ( auto property : properties )
        {
            ss << property << "\n";
        }
        std::string str = ss.str();
        data[1] = str.c_str();
        data[2] = shader.c_str();

        GL_ASSERT( glShaderSource( m_id, 3, data, nullptr ) );
        GL_ASSERT( glCompileShader( m_id ) );
    }

    bool Engine::ShaderObject::check()
    {
        GLint ok;
        std::stringstream error;
        GL_ASSERT( glGetShaderiv( m_id, GL_COMPILE_STATUS, &ok ) );

        if ( !ok )
        {
            error << m_filename << " not compiled.\n";
            error << getShaderInfoLog( m_id );
            CORE_WARN_IF( !ok, error.str().c_str() );
            glDeleteShader( m_id );
        }
        return !( !ok );
    }

    bool Engine::ShaderObject::parseFile( const std::string& filename, std::string& content )
    {
        std::ifstream ifs( filename.c_str(), std::ios::in );
        if ( !ifs )
        {
            return false;
        }

        std::stringstream buf;
        buf << ifs.rdbuf();
        content = buf.str();

        ifs.close();
        return true;
    }

    Engine::ShaderProgram:: ShaderProgram()
        : m_shaderId( 0 )
        , m_binded( false )
    {
        for ( uint i = 0; i < m_shaderObjects.size(); ++i )
        {
            m_shaderObjects[i] = nullptr;
            m_shaderStatus[i]  = false;
        }
    }

    Engine::ShaderProgram::ShaderProgram( const Engine::ShaderConfiguration& config )
        : ShaderProgram()
    {
        load( config );
    }

    Engine::ShaderProgram::~ShaderProgram()
    {
        if ( m_shaderId != 0 )
        {
            for ( auto shader : m_shaderObjects )
            {
                if ( shader && ( shader->getId() != 0 ) )
                {
                    GL_ASSERT( glDetachShader( m_shaderId, shader->getId() ) );
                    delete shader;
                }
            }
            glDeleteProgram( m_shaderId );
        }
    }

    uint Engine::ShaderProgram::getId() const
    {
        return m_shaderId;
    }

    bool Engine::ShaderProgram::isOk() const
    {
        bool ok = true;
        for ( uint i = 0; i < SHADER_TYPE_COUNT; ++i )
        {
            if ( m_configuration.getType() & ( 1 << i ) )
            {
                ok = ( ok && m_shaderStatus[i] );
            }
        }
        return ok;
    }

    void Engine::ShaderProgram::loadVertShader( const std::string& name,
                                                const std::set<std::string>& props )
    {
        Engine::ShaderObject* vertShader = new Engine::ShaderObject;
        bool status = vertShader->loadAndCompile( GL_VERTEX_SHADER, name + ".vert.glsl", props );
        m_shaderObjects[VERT_SHADER] = vertShader;
        m_shaderStatus[VERT_SHADER] = status;
    }

    void Engine::ShaderProgram::loadFragShader( const std::string& name,
                                                const std::set<std::string>& props )
    {
        Engine::ShaderObject* fragShader = new Engine::ShaderObject;
        bool status = fragShader->loadAndCompile( GL_FRAGMENT_SHADER, name + ".frag.glsl", props );
        m_shaderObjects[FRAG_SHADER] = fragShader;
        m_shaderStatus[FRAG_SHADER] = status;
    }

    void Engine::ShaderProgram::loadTessShader( const std::string& name,
                                                const std::set<std::string>& props,
                                                const Engine::ShaderConfiguration::ShaderType& type )
    {
        // TODO
        if ( type & Engine::ShaderConfiguration::TESC_SHADER )
        {
            Engine::ShaderObject* tessCShader = new Engine::ShaderObject;
            bool status = tessCShader->loadAndCompile( GL_TESS_CONTROL_SHADER, name + ".tesc.glsl", props );
            m_shaderObjects[TESC_SHADER] = tessCShader;
            m_shaderStatus[TESC_SHADER] = status;
        }

        if ( type & Engine::ShaderConfiguration::TESE_SHADER )
        {
            Engine::ShaderObject* tessEShader = new Engine::ShaderObject;
            bool status = tessEShader->loadAndCompile( GL_TESS_EVALUATION_SHADER, name + ".tese.glsl", props );
            m_shaderObjects[TESE_SHADER] = tessEShader;
            m_shaderStatus[TESE_SHADER] = status;
        }
    }

    void Engine::ShaderProgram::loadGeomShader( const std::string& name,
                                                const std::set<std::string>& props,
                                                const Engine::ShaderConfiguration::ShaderType& type )
    {
        if ( type & Engine::ShaderConfiguration::GEOM_SHADER )
        {
            Engine::ShaderObject* geomShader = new Engine::ShaderObject;
            bool status = geomShader->loadAndCompile( GL_GEOMETRY_SHADER, name + ".geom.glsl", props );
            m_shaderObjects[GEOM_SHADER] = geomShader;
            m_shaderStatus[GEOM_SHADER] = status;
        }
    }

    void Engine::ShaderProgram::loadCompShader( const std::string& name,
                                                const std::set<std::string>& props,
                                                const Engine::ShaderConfiguration::ShaderType& type )
    {
        if ( type & Engine::ShaderConfiguration::COMP_SHADER )
        {
            Engine::ShaderObject* compShader = new Engine::ShaderObject;
            bool status = compShader->loadAndCompile( GL_COMPUTE_SHADER, name + ".comp.glsl", props );
            m_shaderObjects[COMP_SHADER] = compShader;
            m_shaderStatus[COMP_SHADER] = status;
        }
    }

    void Engine::ShaderProgram::load( const Engine::ShaderConfiguration& shaderConfig )
    {
        //    LOG(INFO) << "Loading shader " << shaderConfig.getName() << " <type = " << std::hex << shaderConfig.getType() << std::dec << ">";
        m_configuration = shaderConfig;

        auto name = shaderConfig.getFullName();
        auto props = shaderConfig.getProperties();
        auto type = shaderConfig.getType();

        GL_ASSERT( m_shaderId = glCreateProgram() );

        loadVertShader( name, props );
        loadTessShader( name, props, type );
        loadGeomShader( name, props, type );
        loadFragShader( name, props );

        link();
    }

    void Engine::ShaderProgram::link()
    {
        for ( auto shader : m_shaderObjects )
        {
            if ( shader )
            {
                GL_ASSERT( glAttachShader( m_shaderId, shader->getId() ) );
            }
        }

        GL_ASSERT( glLinkProgram( m_shaderId ) );
    }

    void Engine::ShaderProgram::bind()
    {
        CORE_ASSERT( m_shaderId != 0, "Shader is not initialized" );
        GL_ASSERT( glUseProgram( m_shaderId ) );
        m_binded = true;
    }

    void Engine::ShaderProgram::unbind()
    {
        GL_ASSERT( glUseProgram( 0 ) );
        m_binded = false;
    }

    void Engine::ShaderProgram::reload()
    {
        for ( unsigned int i = 0; i < m_shaderObjects.size(); ++i )
        {
            if ( m_shaderObjects[i] != nullptr )
            {
                GL_ASSERT( glDetachShader( m_shaderId, m_shaderObjects[i]->getId() ) );
                m_shaderObjects[i]->reloadAndCompile( m_configuration.getProperties() );
            }
        }

        link();
    }

    Engine::ShaderConfiguration Engine::ShaderProgram::getBasicConfiguration() const
    {
        Engine::ShaderConfiguration basicConfig;
        basicConfig.setName( m_configuration.getName() );
        basicConfig.setPath( m_configuration.getPath() );
        basicConfig.setType( m_configuration.getType() );

        return basicConfig;
    }

    void Engine::ShaderProgram::setUniform( const char* name, int value ) const
    {
        GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), value ) );
    }

    void Engine::ShaderProgram::setUniform( const char* name, unsigned int value ) const
    {
        GL_ASSERT( glUniform1ui( glGetUniformLocation( m_shaderId, name ), value ) );
    }

#ifndef CORE_USE_DOUBLE
    void Engine::ShaderProgram::setUniform( const char* name, Scalar value ) const
    {
        GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), value ) );
    }
#else
    void Engine::ShaderProgram::setUniform( const char* name, Scalar value ) const
    {
        GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), static_cast<float>( value ) ) );
    }
#endif

#ifndef CORE_USE_DOUBLE
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector2& value ) const
    {
        GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
    }
#else
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector2& value ) const
    {
        Core::Vector2f v = value.cast<float>();
        GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
    }
#endif

#ifndef CORE_USE_DOUBLE
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector3& value ) const
    {
        GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
    }
#else
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector3& value ) const
    {
        Core::Vector3f v = value.cast<float>();
        GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
    }
#endif

#ifndef CORE_USE_DOUBLE
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector4& value ) const
    {
        GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
    }
#else
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Vector4& value ) const
    {
        Core::Vector4f v = value.cast<float>();
        GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
    }
#endif

#ifndef CORE_USE_DOUBLE
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Matrix2& value ) const
    {
        GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
    }
#else
    void Engine::ShaderProgram::setUniform( const char* name, const Core::Matrix2& value ) const
    {
        Core::Matrix2f v = value.cast<float>();
        GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
    }
#endif

    void Engine::ShaderProgram::setUniform( const char* name, const Core::Matrix3& value ) const
    {
        Core::Matrix3f v = value.cast<float>();
        GL_ASSERT( glUniformMatrix3fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
    }

    void Engine::ShaderProgram::setUniform( const char* name, const Core::Matrix4& value ) const
    {
        Core::Matrix4f v = value.cast<float>();
        GL_ASSERT( glUniformMatrix4fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
    }

    // TODO : Provide Texture support
    void Engine::ShaderProgram::setUniform( const char* name, Engine::Texture* tex, int texUnit ) const
    {
        tex->bind( texUnit );
        GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), texUnit ) );
    }

} // namespace Ra
